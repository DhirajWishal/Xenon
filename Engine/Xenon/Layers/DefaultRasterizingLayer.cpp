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
	}

	void DefaultRasterizingLayer::bind(Layer* pPreviousLayer, Backend::CommandRecorder* pCommandRecorder)
	{
		OPTICK_EVENT();

		pCommandRecorder->bind(m_pRasterizer.get(), { glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, static_cast<uint32_t>(0) }, !m_DrawData.empty());

		for (auto& drawData : m_DrawData)
		{
			auto pchild = m_Renderer.getTaskGraph().create([this, &drawData, pCommandRecorder]
				{
					bindDrawData(drawData, pCommandRecorder);
				}
			);

			pchild->start();
		}

		m_Renderer.getTaskGraph().complete();
		pCommandRecorder->executeChildren();
	}

	void DefaultRasterizingLayer::onUpdate(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex)
	{
		m_pCommandRecorder->begin();
		m_pCommandRecorder->bind(m_pRasterizer.get(), { glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, static_cast<uint32_t>(0) });
		m_pCommandRecorder->setViewport(0.0f, 0.0f, static_cast<float>(m_Renderer.getCamera()->getWidth()), static_cast<float>(m_Renderer.getCamera()->getHeight()), 0.0f, 1.0f);
		m_pCommandRecorder->setScissor(0, 0, m_Renderer.getCamera()->getWidth(), m_Renderer.getCamera()->getHeight());

		for (auto& drawData : m_DrawData)
		{
			m_pCommandRecorder->bind(drawData.m_pPipeline, drawData.m_Storage.getVertexSpecification());
			m_pCommandRecorder->bind(drawData.m_Storage.getVertexBuffer(), drawData.m_Storage.getVertexSpecification().getSize());

			uint32_t materialIndex = 0;
			for (const auto& mesh : drawData.m_Storage.getMeshes())
			{
				for (const auto& subMesh : mesh.m_SubMeshes)
				{
					m_pCommandRecorder->bind(drawData.m_Storage.getIndexBuffer(), static_cast<Backend::IndexBufferStride>(subMesh.m_IndexSize));
					m_pCommandRecorder->bind(drawData.m_pPipeline, nullptr, drawData.m_pMaterialDescriptors[materialIndex].get(), drawData.m_pCameraDescriptor.get());

					m_pCommandRecorder->drawIndexed(subMesh.m_VertexOffset, subMesh.m_IndexOffset, subMesh.m_IndexCount);

					materialIndex++;
				}
			}
		}

		m_pCommandRecorder->end();
	}

	void DefaultRasterizingLayer::addDrawData(MeshStorage&& storage, Backend::RasterizingPipeline* pPipeline)
	{
		OPTICK_EVENT();

		uint32_t subMeshCount = 0;
		for (const auto& mesh : storage.getMeshes())
			subMeshCount += mesh.m_SubMeshes.size();

		// Create a new draw entry.
		DrawData drawEntry = {};
		drawEntry.m_Storage = std::move(storage);
		drawEntry.m_pPipeline = pPipeline;
		// drawEntry.m_pCommandRecorder = m_Renderer.getInstance().getFactory()->createCommandRecorder(m_Renderer.getInstance().getBackendDevice(), Backend::CommandRecorderUsage::Secondary, subMeshCount * m_Renderer.getCommandRecorder()->getBufferCount());

		// Setup the camera descriptor.
		drawEntry.m_pCameraDescriptor = pPipeline->createDescriptor(Backend::DescriptorType::Camera);
		drawEntry.m_pCameraDescriptor->attach(0, m_Renderer.getCamera()->getViewports().front().m_pUniformBuffer);

		// Setup the material descriptors.
		drawEntry.m_pMaterialDescriptors.reserve(subMeshCount);
		for (const auto& mesh : drawEntry.m_Storage.getMeshes())
		{
			for (const auto& subMesh : mesh.m_SubMeshes)
				drawEntry.m_pMaterialDescriptors.emplace_back(subMesh.m_MaterialIdentifier.m_pMaterial->createDescriptor(pPipeline));
		}

		// Insert the created draw entry.
		m_DrawData.emplace_back(std::move(drawEntry));
	}

	void DefaultRasterizingLayer::bindDrawData(DrawData& drawData, Backend::CommandRecorder* pCommandRecorder) const
	{
		OPTICK_EVENT();

		uint32_t materialIndex = 0;
		for (const auto& mesh : drawData.m_Storage.getMeshes())
		{
			for (const auto& subMesh : mesh.m_SubMeshes)
			{
				drawData.m_pCommandRecorder->begin(pCommandRecorder);
				drawData.m_pCommandRecorder->bind(drawData.m_pPipeline, drawData.m_Storage.getVertexSpecification());
				drawData.m_pCommandRecorder->bind(drawData.m_Storage.getVertexBuffer(), drawData.m_Storage.getVertexSpecification().getSize());
				drawData.m_pCommandRecorder->bind(drawData.m_Storage.getIndexBuffer(), static_cast<Backend::IndexBufferStride>(subMesh.m_IndexSize));
				drawData.m_pCommandRecorder->bind(drawData.m_pPipeline, nullptr, drawData.m_pMaterialDescriptors[materialIndex].get(), drawData.m_pCameraDescriptor.get());

				drawData.m_pCommandRecorder->setViewport(0.0f, 0.0f, static_cast<float>(m_Renderer.getCamera()->getWidth()), static_cast<float>(m_Renderer.getCamera()->getHeight()), 0.0f, 1.0f);
				drawData.m_pCommandRecorder->setScissor(0, 0, m_Renderer.getCamera()->getWidth(), m_Renderer.getCamera()->getHeight());

				drawData.m_pCommandRecorder->drawIndexed(subMesh.m_VertexOffset, subMesh.m_IndexOffset, subMesh.m_IndexCount);

				drawData.m_pCommandRecorder->end();
				drawData.m_pCommandRecorder->next();

				materialIndex++;
			}
		}
	}
}