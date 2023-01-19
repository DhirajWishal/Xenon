// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "RayTracingLayer.hpp"
#include "Renderer.hpp"

namespace Xenon
{
	RayTracingLayer::RayTracingLayer(Renderer& renderer, uint32_t priority, Backend::Camera* pCamera)
		: Layer(renderer, priority)
		, m_pRayTracer(renderer.getInstance().getFactory()->createRayTracer(renderer.getInstance().getBackendDevice(), pCamera))
	{
	}

	Xenon::Backend::Image* RayTracingLayer::getColorAttachment()
	{
		return m_pRayTracer->getImageAttachment(Backend::AttachmentType::Color);
	}
}