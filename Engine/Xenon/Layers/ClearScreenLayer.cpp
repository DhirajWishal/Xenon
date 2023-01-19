// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ClearScreenLayer.hpp"
#include "../Renderer.hpp"

#include <optick.h>

namespace Xenon
{
	ClearScreenLayer::ClearScreenLayer(Renderer& renderer, Backend::Camera* pCamera, const glm::vec4& color, uint32_t priority)
		: RasterizingLayer(renderer, priority, pCamera, Backend::AttachmentType::Color)
		, m_ClearColor(color)
	{
	}

	void ClearScreenLayer::onUpdate(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex)
	{
		OPTICK_EVENT();

		m_pCommandRecorder->begin();
		m_pCommandRecorder->bind(m_pRasterizer.get(), { m_ClearColor });
		m_pCommandRecorder->end();
	}
}