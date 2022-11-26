// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DefaultRasterizingLayer.hpp"

#include <glm/vec4.hpp>

namespace Xenon
{
	DefaultRasterizingLayer::DefaultRasterizingLayer(Instance& instance, Backend::Camera* pCamera)
		: RasterizingLayer(instance, pCamera, Backend::AttachmentType::Color | Backend::AttachmentType::Depth | Backend::AttachmentType::Stencil)
	{
	}

	void DefaultRasterizingLayer::bind(Layer* pPreviousLayer, Backend::CommandRecorder* pCommandRecorder)
	{
		pCommandRecorder->bind(m_pRasterizer.get(), { glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 0.0f, static_cast<uint32_t>(0) });

		for (const auto& drawData : m_DrawData)
		{
			pCommandRecorder->bind(drawData.m_pPipeline, drawData.m_Storage.getVertexSpecification());
		}
	}
}