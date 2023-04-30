// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#include "RayTracingLayer.hpp"
#include "Renderer.hpp"

namespace Xenon
{
	RayTracingLayer::RayTracingLayer(Renderer& renderer, uint32_t priority, uint32_t width, uint32_t height)
		: Layer(renderer, priority)
		, m_pRayTracer(renderer.getInstance().getFactory()->createRayTracer(renderer.getInstance().getBackendDevice(), width, height))
	{
	}

	Xenon::Backend::Image* RayTracingLayer::getColorAttachment()
	{
		return m_pRayTracer->getImageAttachment(Backend::AttachmentType::Color);
	}
}