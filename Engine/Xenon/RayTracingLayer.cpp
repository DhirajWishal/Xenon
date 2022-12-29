// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "RayTracingLayer.hpp"
#include "Renderer.hpp"

namespace Xenon
{
	RayTracingLayer::RayTracingLayer(Renderer& renderer, Backend::Camera* pCamera)
		: Layer(renderer)
		, m_pRayTracer(renderer.getInstance().getFactory()->createRayTracer(renderer.getInstance().getBackendDevice(), pCamera))
	{
	}

	Xenon::Backend::Image* RayTracingLayer::getColorAttachment()
	{
		return m_pRayTracer->getImageAttachment(Backend::AttachmentType::Color);
	}
}