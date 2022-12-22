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
	{
		m_Workers.reserve(GetUsableThreadCount());
		for (uint8_t i = 0; i < GetUsableThreadCount(); i++)
			m_Workers.emplace_back([this, i] { subMeshBinder(i); });
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

		m_pCommandRecorder->begin();
		m_pCommandRecorder->bind(m_pRasterizer.get(), { glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, static_cast<uint32_t>(0) }, true);

		m_Synchronization.reset(m_Workers.size());
		m_ConditionVariable.notify_all();
		m_Synchronization.wait();

		m_pCommandRecorder->executeChildren();
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

				pCommandRecorder->bind(entry.m_pPipeline, entry.m_VertexSpecification);
				if (lock) lock.unlock();

				pCommandRecorder->bind(entry.m_pVertexBuffer, entry.m_VertexSpecification.getSize());
				pCommandRecorder->bind(entry.m_pIndexBuffer, static_cast<Backend::IndexBufferStride>(entry.m_SubMesh.m_IndexSize));
				pCommandRecorder->bind(entry.m_pPipeline, entry.m_pUserDefinedDescriptor, entry.m_pMaterialDescriptor.get(), entry.m_pCameraDescriptor);

				pCommandRecorder->drawIndexed(entry.m_SubMesh.m_VertexOffset, entry.m_SubMesh.m_IndexOffset, entry.m_SubMesh.m_IndexCount);

				if (!lock) lock.lock();
			}

			// End the command recorder.
			pCommandRecorder->end();

			// Select the next command recorder.
			pCommandRecorder->next();

			// Notify the parent that we're done.
			m_Synchronization.arrive();
		}
	}
}