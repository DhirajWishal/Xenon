// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ClearScreenLayer.hpp"
#include "../Renderer.hpp"

#include <optick.h>

namespace Xenon
{
	ClearScreenLayer::ClearScreenLayer(Renderer& renderer, Backend::Camera* pCamera, const glm::vec4& color)
		: RasterizingLayer(renderer, pCamera, Backend::AttachmentType::Color)
		, m_ClearColor(color)
	{
	}

	void ClearScreenLayer::bind(Layer* pPreviousLayer, Backend::CommandRecorder* pCommandRecorder)
	{
		OPTICK_EVENT();

		pCommandRecorder->bind(m_pRasterizer.get(), { m_ClearColor });
	}
}