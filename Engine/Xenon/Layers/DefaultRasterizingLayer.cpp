// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DefaultRasterizingLayer.hpp"
#include "../Renderer.hpp"

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
		m_DrawData.emplace_back(
			std::move(storage),
			pPipeline,
			m_Renderer.getInstance().getFactory()->createCommandRecorder(m_Renderer.getInstance().getBackendDevice(), Backend::CommandRecorderUsage::Secondary, 3)
		);
	}

	void DefaultRasterizingLayer::bindDrawData(DrawData& drawData, Backend::CommandRecorder* pCommandRecorder) const
	{
		drawData.m_pCommandRecorder->begin(pCommandRecorder);
		drawData.m_pCommandRecorder->bind(drawData.m_pPipeline, drawData.m_Storage.getVertexSpecification());

		for (const auto& mesh : drawData.m_Storage.getMeshes())
		{
			for (const auto& subMesh : mesh.m_SubMeshes)
			{
				drawData.m_pCommandRecorder->bind(drawData.m_Storage.getVertexBuffer(), drawData.m_Storage.getVertexSpecification().getSize(), drawData.m_Storage.getIndexBuffer(), subMesh.m_IndexSize);
				drawData.m_pCommandRecorder->bind(drawData.m_pPipeline, nullptr, subMesh.m_pMaterial->m_pDescriptor.get(), m_Renderer.getCameraDescriptor());
				drawData.m_pCommandRecorder->drawIndexed(subMesh.m_VertexOffset, subMesh.m_IndexOffset, subMesh.m_IndexCount);
			}
		}

		drawData.m_pCommandRecorder->end();
		drawData.m_pCommandRecorder->next();
	}
}