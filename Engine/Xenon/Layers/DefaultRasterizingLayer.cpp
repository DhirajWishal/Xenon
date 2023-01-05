// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DefaultRasterizingLayer.hpp"
#include "../Renderer.hpp"
#include "../../XenonCore/Logging.hpp"

#include <optick.h>
#include <glm/vec4.hpp>

// #define ENABLE_OCCLUSION_CULL

namespace Xenon
{
	DefaultRasterizingLayer::DefaultRasterizingLayer(Renderer& renderer, Backend::Camera* pCamera)
		: RasterizingLayer(renderer, pCamera, Backend::AttachmentType::Color | Backend::AttachmentType::Depth | Backend::AttachmentType::Stencil)
		, m_pOcclusionQuery(renderer.getInstance().getFactory()->createOcclusionQuery(m_Renderer.getInstance().getBackendDevice(), 1))
	{
		// Setup the occlusion pipeline.
		setupOcclusionPipeline();
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

#ifdef ENABLE_OCCLUSION_CULL
		// Reset the query.
		m_pCommandRecorder->resetQuery(m_pOcclusionQuery.get());

#endif // ENABLE_OCCLUSION_CULL

		// Bind the render target.
		m_pCommandRecorder->bind(m_pRasterizer.get(), { glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, static_cast<uint32_t>(0) }, true);

		// Issue the draw calls.
		issueDrawCalls();

#ifdef ENABLE_OCCLUSION_CULL
		// Query the results only if we have drawn something.
		if (m_SubMeshCount > 0)
			m_pCommandRecorder->getQueryResults(m_pOcclusionQuery.get());

#endif // ENABLE_OCCLUSION_CULL

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
		drawData.m_pSceneDescriptor = pPipeline->createDescriptor(Backend::DescriptorType::Scene);
		drawData.m_pSceneDescriptor->attach(0, m_Renderer.getCamera()->getViewports().front().m_pUniformBuffer);

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
				entry.m_pSceneDescriptor = drawData.m_pSceneDescriptor.get();
				entry.m_QueryIndex = m_SubMeshCount++;

				threadIndex++;
			}
		}

		// Insert the newly created draw data.
		m_DrawData.emplace_back(std::move(drawData));
	}

	void DefaultRasterizingLayer::setupOcclusionPipeline()
	{
#ifdef ENABLE_OCCLUSION_CULL
		// Create the pipeline.
		Backend::RasterizingPipelineSpecification specification = {};
		specification.m_VertexShader = Backend::ShaderSource::FromFile(XENON_SHADER_DIR "Occlusion/Shader.vert.spv");
		// specification.m_FragmentShader = Backend::ShaderSource::FromFile(XENON_SHADER_DIR "Occlusion/Shader.frag.spv");

		m_pOcclusionPipeline = m_Renderer.getInstance().getFactory()->createRasterizingPipeline(m_Renderer.getInstance().getBackendDevice(), nullptr, m_pRasterizer.get(), specification);

		// Setup the occlusion camera descriptor.
		m_pOcclusionCameraDescriptor = m_pOcclusionPipeline->createDescriptor(Backend::DescriptorType::Camera);
		m_pOcclusionCameraDescriptor->attach(0, m_Renderer.getCamera()->getViewports().front().m_pUniformBuffer);

#endif // ENABLE_OCCLUSION_CULL
	}

	void DefaultRasterizingLayer::issueDrawCalls()
	{
		OPTICK_EVENT();

		// Reset the counters.
		m_DrawCount = 0;
		m_Synchronization.reset(m_SubMeshCount);

		// Begin the command recorders and set the viewport and scissor.
		for (const auto& [id, pCommandRecorder] : m_pThreadLocalCommandRecorder)
		{
			OPTICK_EVENT_DYNAMIC("Begin Secondary Recorders");

			// Begin the command recorder.
			pCommandRecorder->begin(m_pCommandRecorder.get());

			// Set the scissor and view port.
			pCommandRecorder->setViewport(0.0f, 0.0f, static_cast<float>(m_Renderer.getCamera()->getWidth()), static_cast<float>(m_Renderer.getCamera()->getHeight()), 0.0f, 1.0f);
			pCommandRecorder->setScissor(0, 0, m_Renderer.getCamera()->getWidth(), m_Renderer.getCamera()->getHeight());
		}

		// Issue the binding calls.
		for (const auto& drawEntry : m_DrawEntries)
		{
			OPTICK_EVENT_DYNAMIC("Binding Draw Entries");

			for (const auto& entry : drawEntry)
			{
				OPTICK_EVENT_DYNAMIC("Binding Entry");

				GetJobSystem().insert([this, &entry] { bindingCall(entry); });
			}
		}

		// Wait till all the work is done.
		m_Synchronization.wait();

		// End the command recorders and select the next one.
		for (const auto& [id, pCommandRecorder] : m_pThreadLocalCommandRecorder)
		{
			OPTICK_EVENT_DYNAMIC("End Secondary Recorders");

			// End the command recorder.
			pCommandRecorder->end();

			// Select the next command recorder.
			pCommandRecorder->next();
		}

		// Execute all the secondary command recorders (children).
		m_pCommandRecorder->executeChildren();
	}

	void DefaultRasterizingLayer::bindingCall(const DrawEntry& entry)
	{
		OPTICK_EVENT();

		// Setup the command recorders if we don't have one for this thread.
		const auto id = std::this_thread::get_id();
		if (!m_pThreadLocalCommandRecorder.contains(id))
		{
			auto lock = std::scoped_lock(m_Mutex);

			// Create the thread-specific command recorder.
			const auto& pCommandRecorder = m_pThreadLocalCommandRecorder[id] = m_Renderer.getInstance().getFactory()->createCommandRecorder(
				m_Renderer.getInstance().getBackendDevice(),
				Backend::CommandRecorderUsage::Secondary,
				m_Renderer.getCommandRecorder()->getBufferCount()
			);

			// Begin the command recorder.
			pCommandRecorder->begin(m_pCommandRecorder.get());

			// Set the scissor and view port.
			pCommandRecorder->setViewport(0.0f, 0.0f, static_cast<float>(m_Renderer.getCamera()->getWidth()), static_cast<float>(m_Renderer.getCamera()->getHeight()), 0.0f, 1.0f);
			pCommandRecorder->setScissor(0, 0, m_Renderer.getCamera()->getWidth(), m_Renderer.getCamera()->getHeight());
		}

#ifdef ENABLE_OCCLUSION_CULL
		// Run the occlusion pass on the entry.
		occlusionPass(entry);

#endif // ENABLE_OCCLUSION_CULL

		// Run the geometry pass on the entry.
		geometryPass(entry);

		// Notify the parent that we're done.
		m_Synchronization.arrive();
	}

	void DefaultRasterizingLayer::occlusionPass(const DrawEntry& entry) const
	{
		OPTICK_EVENT();

		const auto& pCommandRecorder = m_pThreadLocalCommandRecorder.at(std::this_thread::get_id());

		pCommandRecorder->bind(m_pOcclusionPipeline.get(), entry.m_VertexSpecification);
		pCommandRecorder->bind(entry.m_pVertexBuffer, entry.m_VertexSpecification.getSize());
		pCommandRecorder->bind(entry.m_pIndexBuffer, static_cast<Backend::IndexBufferStride>(entry.m_SubMesh.m_IndexSize));
		pCommandRecorder->bind(m_pOcclusionPipeline.get(), nullptr, nullptr, m_pOcclusionCameraDescriptor.get());

		pCommandRecorder->beginQuery(m_pOcclusionQuery.get(), static_cast<uint32_t>(entry.m_QueryIndex));
		pCommandRecorder->drawIndexed(entry.m_SubMesh.m_VertexOffset, entry.m_SubMesh.m_IndexOffset, entry.m_SubMesh.m_IndexCount);
		pCommandRecorder->endQuery(m_pOcclusionQuery.get(), static_cast<uint32_t>(entry.m_QueryIndex));
	}

	void DefaultRasterizingLayer::geometryPass(const DrawEntry& entry)
	{
		OPTICK_EVENT();

		const auto& pCommandRecorder = m_pThreadLocalCommandRecorder.at(std::this_thread::get_id());

#ifdef ENABLE_OCCLUSION_CULL
		// Draw only if we're in the camera's view frustum.
		if (m_pOcclusionQuery->getSamples()[entry.m_QueryIndex] > 0)

#endif // ENABLE_OCCLUSION_CULL

		{
			OPTICK_EVENT_DYNAMIC("Issuing Draw Calls");

			pCommandRecorder->bind(entry.m_pPipeline, entry.m_VertexSpecification);
			pCommandRecorder->bind(entry.m_pVertexBuffer, entry.m_VertexSpecification.getSize());
			pCommandRecorder->bind(entry.m_pIndexBuffer, static_cast<Backend::IndexBufferStride>(entry.m_SubMesh.m_IndexSize));
			pCommandRecorder->bind(entry.m_pPipeline, entry.m_pUserDefinedDescriptor, entry.m_pMaterialDescriptor.get(), entry.m_pSceneDescriptor);

			pCommandRecorder->drawIndexed(entry.m_SubMesh.m_VertexOffset, entry.m_SubMesh.m_IndexOffset, entry.m_SubMesh.m_IndexCount);

			m_DrawCount++;
		}
	}
}