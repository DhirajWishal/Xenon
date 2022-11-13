// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "RasterizingLayer.hpp"

namespace Xenon
{
	RasterizingLayer::RasterizingLayer(Instance& instance, uint8_t priority, Backend::Camera* pCamera, Backend::AttachmentType attachmentTypes, bool enableTripleBuffering = false, Backend::MultiSamplingCount multiSampleCount = Backend::MultiSamplingCount::x1)
		: Layer(instance, priority)
		, m_pRasterizer(instance.getFactory()->createRasterizer(instance.getBackendDevice(), pCamera, attachmentTypes, enableTripleBuffering, multiSampleCount))
	{
	}
}