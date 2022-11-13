// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ClearScreenLayer.hpp"

namespace Xenon
{
	ClearScreenLayer::ClearScreenLayer(Instance& instance, Backend::Camera* pCamera, const glm::vec4& color)
		: RasterizingLayer(instance, pCamera, Backend::AttachmentType::Color)
		, m_ClearColor(color)
	{
	}

	void ClearScreenLayer::bind(Layer* pPreviousLayer, Backend::CommandRecorder* pCommandRecorder)
	{
		pCommandRecorder->bind(m_pRasterizer.get(), { m_ClearColor });
	}
}