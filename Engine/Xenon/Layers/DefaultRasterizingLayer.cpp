// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DefaultRasterizingLayer.hpp"
#include "../Renderer.hpp"
#include "../../XenonCore/Logging.hpp"

#include <glm/vec4.hpp>

namespace Xenon
{
	DefaultRasterizingLayer::DefaultRasterizingLayer(Renderer& renderer, Backend::Camera* pCamera)
		: RasterizingLayer(renderer, pCamera, Backend::AttachmentType::Color | Backend::AttachmentType::Depth | Backend::AttachmentType::Stencil)
	{
	}

	void DefaultRasterizingLayer::bind(Layer* pPreviousLayer, Backend::CommandRecorder* pCommandRecorder)
	{
		pCommandRecorder->bind(m_pRasterizer.get(), { glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 0.0f, static_cast<uint32_t>(0) }, !m_DrawData.empty());

		for (auto& drawData : m_DrawData)
		{
			auto pchild = m_Renderer.getTaskGraph().create([this, &drawData, pCommandRecorder]
				{
					bindDrawData(drawData, pCommandRecorder);
				}
			);
		}

		m_Renderer.getTaskGraph().complete();
		pCommandRecorder->executeChildren();
	}

	void DefaultRasterizingLayer::addDrawData(MeshStorage&& storage, Backend::RasterizingPipeline* pPipeline)
	{
		// Create a new draw entry.
		auto& drawEntry = m_DrawData.emplace_back(
			std::move(storage),
			pPipeline,
			m_Renderer.getInstance().getFactory()->createCommandRecorder(m_Renderer.getInstance().getBackendDevice(), Backend::CommandRecorderUsage::Secondary, 3)
		);

		// Setup the camera descriptor.
		drawEntry.m_pCameraDescriptor = pPipeline->createDescriptor(Backend::DescriptorType::Camera);
		drawEntry.m_pCameraDescriptor->attach(0, m_Renderer.getCamera()->getViewports().front().m_pUniformBuffer);

		// Setup the material descriptors.
		for (const auto& mesh : drawEntry.m_Storage.getMeshes())
		{
			for (const auto& subMesh : mesh.m_SubMeshes)
			{
				drawEntry.m_pMaterialDescriptors.emplace_back(subMesh.m_MaterialIdentifier.m_pMaterial->createDescriptor(pPipeline));

			}
		}
	}

	void DefaultRasterizingLayer::bindDrawData(DrawData& drawData, Backend::CommandRecorder* pCommandRecorder) const
	{
		drawData.m_pCommandRecorder->begin(pCommandRecorder);
		drawData.m_pCommandRecorder->bind(drawData.m_pPipeline, drawData.m_Storage.getVertexSpecification());
		drawData.m_pCommandRecorder->bind(drawData.m_Storage.getVertexBuffer(), drawData.m_Storage.getVertexSpecification().getSize());

		uint32_t materialIndex = 0;
		for (const auto& mesh : drawData.m_Storage.getMeshes())
		{
			for (const auto& subMesh : mesh.m_SubMeshes)
			{
				drawData.m_pCommandRecorder->bind(drawData.m_Storage.getIndexBuffer(), static_cast<Backend::IndexBufferStride>(subMesh.m_IndexSize));
				drawData.m_pCommandRecorder->bind(drawData.m_pPipeline, nullptr, drawData.m_pMaterialDescriptors[materialIndex].get(), drawData.m_pCameraDescriptor.get());
				drawData.m_pCommandRecorder->drawIndexed(subMesh.m_VertexOffset, subMesh.m_IndexOffset, subMesh.m_IndexCount);
				materialIndex++;
			}
		}

		drawData.m_pCommandRecorder->end();
		drawData.m_pCommandRecorder->next();
	}
}