// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DefaultRasterizingLayer.hpp"
#include "../Renderer.hpp"
#include "../../XenonCore/Logging.hpp"

#include <optick.h>
#include <glm/vec4.hpp>

namespace Xenon
{
	DefaultRasterizingLayer::DefaultRasterizingLayer(Renderer& renderer, Backend::Camera* pCamera)
		: RasterizingLayer(renderer, pCamera, Backend::AttachmentType::Color | Backend::AttachmentType::Depth | Backend::AttachmentType::Stencil)
		, m_pOcclusionQuery(renderer.getInstance().getFactory()->createOcclusionQuery(m_Renderer.getInstance().getBackendDevice(), 1))
	{
		// Setup the workers.
		m_Workers.reserve(GetUsableThreadCount());
		for (uint8_t i = 0; i < GetUsableThreadCount(); i++)
			m_Workers.emplace_back([this, i] { subMeshBinder(i); });

		// Setup the occlusion pipeline.
		setupOcclusionPipeline();
	}

	DefaultRasterizingLayer::~DefaultRasterizingLayer()
	{
		m_bShouldRun = false;
		m_ConditionVariable.notify_all();
	}

	void DefaultRasterizingLayer::bind(Layer* pPreviousLayer, Backend::CommandRecorder* pCommandRecorder)
	{
		OPTICK_EVENT();

		pCommandRecorder->bind(m_pRasterizer.get(), { glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, static_cast<uint32_t>(0) }, !m_DrawData.empty());
		pCommandRecorder->executeChildren();
	}

	void DefaultRasterizingLayer::onUpdate(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex)
	{
		OPTICK_EVENT();

		// Begin recording.
		m_pCommandRecorder->begin();

		// Re-create the occlusion query if needed.
		{
			auto lock = std::scoped_lock(m_Mutex);
			if (m_SubMeshCount > 0 && m_pOcclusionQuery->getSampleCount() != m_SubMeshCount)
			{
				m_Renderer.getInstance().getBackendDevice()->waitIdle();
				m_pOcclusionQuery = m_Renderer.getInstance().getFactory()->createOcclusionQuery(m_Renderer.getInstance().getBackendDevice(), m_SubMeshCount);
			}
		}

		// Reset the query.
		m_pCommandRecorder->resetQuery(m_pOcclusionQuery.get());

		// Bind the render target.
		m_pCommandRecorder->bind(m_pRasterizer.get(), { glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, static_cast<uint32_t>(0) }, true);

		// Run the workers and wait till they're executed.
		m_DrawCount = 0;
		m_Synchronization.reset(m_Workers.size());
		m_ConditionVariable.notify_all();
		m_Synchronization.wait();

		// Execute all the secondary command recorders (children).
		m_pCommandRecorder->executeChildren();

		// Query the results only if we have drawn something.
		if (m_SubMeshCount > 0)
			m_pCommandRecorder->getQueryResults(m_pOcclusionQuery.get());

		// End the command recorder recording.
		m_pCommandRecorder->end();
	}

	void DefaultRasterizingLayer::addDrawData(MeshStorage&& storage, Backend::RasterizingPipeline* pPipeline)
	{
		OPTICK_EVENT();

		// Create a new draw entry.
		DrawData drawData;
		drawData.m_Storage = std::move(storage);

		// Setup the camera descriptor.
		drawData.m_pCameraDescriptor = pPipeline->createDescriptor(Backend::DescriptorType::Camera);
		drawData.m_pCameraDescriptor->attach(0, m_Renderer.getCamera()->getViewports().front().m_pUniformBuffer);

		// Wait if we're in the process of rendering.
		m_Synchronization.wait();

		// Lock the resources for loading.
		auto lock = std::scoped_lock(m_Mutex);

		// Setup the material descriptors.
		uint32_t threadIndex = 0;
		for (const auto& mesh : drawData.m_Storage.getMeshes())
		{
			for (const auto& subMesh : mesh.m_SubMeshes)
			{
				auto& entry = m_DrawEntries[threadIndex % m_DrawEntries.size()].emplace_back();
				entry.m_SubMesh = subMesh;
				entry.m_VertexSpecification = drawData.m_Storage.getVertexSpecification();
				entry.m_pPipeline = pPipeline;
				entry.m_pVertexBuffer = drawData.m_Storage.getVertexBuffer();
				entry.m_pIndexBuffer = drawData.m_Storage.getIndexBuffer();
				entry.m_pUserDefinedDescriptor = nullptr;
				entry.m_pMaterialDescriptor = subMesh.m_MaterialIdentifier.m_pMaterial->createDescriptor(pPipeline);
				entry.m_pCameraDescriptor = drawData.m_pCameraDescriptor.get();
				entry.m_QueryIndex = m_SubMeshCount++;

				threadIndex++;
			}
		}

		// Insert the newly created draw data.
		m_DrawData.emplace_back(std::move(drawData));
	}

	void DefaultRasterizingLayer::subMeshBinder(uint8_t index)
	{
		const auto threadID = fmt::format("DefaultRasterizingLayer::subMeshBinder({})", index);
		OPTICK_THREAD(threadID.c_str());

		// Setup the secondary command recorder.
		auto pCommandRecorder = m_Renderer.getInstance().getFactory()->createCommandRecorder(
			m_Renderer.getInstance().getBackendDevice(),
			Backend::CommandRecorderUsage::Secondary,
			m_Renderer.getCommandRecorder()->getBufferCount()
		);

		const auto& entries = m_DrawEntries[index];
		auto lock = std::unique_lock(m_Mutex);

		while (true)
		{
			// Wait till we get the signal to go.
			m_ConditionVariable.wait(lock);

			// Return from the function if we need to terminate.
			if (m_bShouldRun == false)
				return;

			OPTICK_EVENT_DYNAMIC("Binding Sub-Meshes");

			// Begin the command recorder.
			pCommandRecorder->begin(m_pCommandRecorder.get());

			pCommandRecorder->setViewport(0.0f, 0.0f, static_cast<float>(m_Renderer.getCamera()->getWidth()), static_cast<float>(m_Renderer.getCamera()->getHeight()), 0.0f, 1.0f);
			pCommandRecorder->setScissor(0, 0, m_Renderer.getCamera()->getWidth(), m_Renderer.getCamera()->getHeight());

			for (const auto& entry : entries)
			{
				OPTICK_EVENT_DYNAMIC("Binding Sub-Mesh");

				occlusionPass(pCommandRecorder.get(), lock, entry);
				geometryPass(pCommandRecorder.get(), lock, entry);
			}

			// End the command recorder.
			pCommandRecorder->end();

			// Select the next command recorder.
			pCommandRecorder->next();

			// Notify the parent that we're done.
			m_Synchronization.arrive();
		}
	}

	void DefaultRasterizingLayer::setupOcclusionPipeline()
	{
		// Create the pipeline.
		Backend::RasterizingPipelineSpecification specification = {};
		specification.m_VertexShader = Backend::ShaderSource::FromFile(XENON_SHADER_DIR "Occlusion/Shader.vert.spv");
		// specification.m_FragmentShader = Backend::ShaderSource::FromFile(XENON_SHADER_DIR "Occlusion/Shader.frag.spv");

		m_pOcclusionPipeline = m_Renderer.getInstance().getFactory()->createRasterizingPipeline(m_Renderer.getInstance().getBackendDevice(), nullptr, m_pRasterizer.get(), specification);

		// Setup the occlusion camera descriptor.
		m_pOcclusionCameraDescriptor = m_pOcclusionPipeline->createDescriptor(Backend::DescriptorType::Camera);
		m_pOcclusionCameraDescriptor->attach(0, m_Renderer.getCamera()->getViewports().front().m_pUniformBuffer);
	}

	void DefaultRasterizingLayer::occlusionPass(Backend::CommandRecorder* pCommandRecorder, std::unique_lock<std::mutex>& lock, const DrawEntry& entry) const
	{
		OPTICK_EVENT();

		pCommandRecorder->bind(m_pOcclusionPipeline.get(), entry.m_VertexSpecification);
		if (lock) lock.unlock();

		pCommandRecorder->bind(entry.m_pVertexBuffer, entry.m_VertexSpecification.getSize());
		pCommandRecorder->bind(entry.m_pIndexBuffer, static_cast<Backend::IndexBufferStride>(entry.m_SubMesh.m_IndexSize));
		pCommandRecorder->bind(m_pOcclusionPipeline.get(), nullptr, nullptr, m_pOcclusionCameraDescriptor.get());

		pCommandRecorder->beginQuery(m_pOcclusionQuery.get(), static_cast<uint32_t>(entry.m_QueryIndex));
		pCommandRecorder->drawIndexed(entry.m_SubMesh.m_VertexOffset, entry.m_SubMesh.m_IndexOffset, entry.m_SubMesh.m_IndexCount);
		pCommandRecorder->endQuery(m_pOcclusionQuery.get(), static_cast<uint32_t>(entry.m_QueryIndex));

		if (!lock) lock.lock();
	}

	void DefaultRasterizingLayer::geometryPass(Backend::CommandRecorder* pCommandRecorder, std::unique_lock<std::mutex>& lock, const DrawEntry& entry)
	{
		OPTICK_EVENT();

		// Draw only if we're in the camera's view frustum.
		if (m_pOcclusionQuery->getSamples()[entry.m_QueryIndex] > 0)
		{
			pCommandRecorder->bind(entry.m_pPipeline, entry.m_VertexSpecification);
			if (lock) lock.unlock();

			pCommandRecorder->bind(entry.m_pVertexBuffer, entry.m_VertexSpecification.getSize());
			pCommandRecorder->bind(entry.m_pIndexBuffer, static_cast<Backend::IndexBufferStride>(entry.m_SubMesh.m_IndexSize));
			pCommandRecorder->bind(entry.m_pPipeline, entry.m_pUserDefinedDescriptor, entry.m_pMaterialDescriptor.get(), entry.m_pCameraDescriptor);

			pCommandRecorder->drawIndexed(entry.m_SubMesh.m_VertexOffset, entry.m_SubMesh.m_IndexOffset, entry.m_SubMesh.m_IndexCount);

			if (!lock) lock.lock();
			m_DrawCount++;
		}
	}
}