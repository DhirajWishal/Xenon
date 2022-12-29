// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DefaultRayTracingLayer.hpp"
#include "../Renderer.hpp"

namespace Xenon
{
	DefaultRayTracingLayer::DefaultRayTracingLayer(Renderer& renderer, Backend::Camera* pCamera)
		: RayTracingLayer(renderer, pCamera)
	{

	}

	void DefaultRayTracingLayer::onUpdate(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex)
	{

	}

	void DefaultRayTracingLayer::addDrawData(MeshStorage&& storage/*, Backend::RasterizingPipeline* pPipeline*/)
	{

	}
}