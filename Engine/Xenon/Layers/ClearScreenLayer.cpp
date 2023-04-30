// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#include "ClearScreenLayer.hpp"
#include "../Renderer.hpp"

#include <optick.h>

namespace Xenon
{
	ClearScreenLayer::ClearScreenLayer(Renderer& renderer, uint32_t width, uint32_t height, const glm::vec4& color, uint32_t priority)
		: RasterizingLayer(renderer, priority, width, height, Backend::AttachmentType::Color)
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