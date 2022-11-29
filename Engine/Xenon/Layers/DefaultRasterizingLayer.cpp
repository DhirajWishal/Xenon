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
		if (!m_DrawData.empty())
		{
			int i = 0;
			i = 1000;
		}

		pCommandRecorder->bind(m_pRasterizer.get(), { glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 0.0f, static_cast<uint32_t>(0) }, !m_DrawData.empty());

		for (const auto& drawData : m_DrawData)
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

	void DefaultRasterizingLayer::bindDrawData(const DrawData& drawData, Backend::CommandRecorder* pCommandRecorder) const
	{
		drawData.m_pCommandRecorder->begin(pCommandRecorder);
		drawData.m_pCommandRecorder->bind(drawData.m_pPipeline, drawData.m_Storage.getVertexSpecification());
		drawData.m_pCommandRecorder->end();
		drawData.m_pCommandRecorder->next();
	}
}