// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DefaultRayTracingLayer.hpp"
#include "../Renderer.hpp"

#include <optick.h>

namespace Xenon
{
	DefaultRayTracingLayer::DefaultRayTracingLayer(Renderer& renderer, Backend::Camera* pCamera)
		: RayTracingLayer(renderer, pCamera)
	{
	}

	void DefaultRayTracingLayer::onUpdate(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex)
	{
		OPTICK_EVENT();

		m_pCommandRecorder->begin();

		const auto lock = std::scoped_lock(m_Mutex);
		for (const auto& drawData : m_DrawData)
		{
			m_pCommandRecorder->drawRayTraced(m_pRayTracer.get(), drawData.m_pPipeline, nullptr);
		}

		m_pCommandRecorder->end();
	}

	void DefaultRayTracingLayer::addDrawData(MeshStorage&& storage, Backend::RayTracingPipeline* pPipeline)
	{
		OPTICK_EVENT();

		Backend::AccelerationStructureGeometry geometry;
		geometry.m_VertexSpecification = storage.getVertexSpecification();
		geometry.m_pVertexBuffer = storage.getVertexBuffer();
		geometry.m_pIndexBuffer = storage.getIndexBuffer();
		geometry.m_IndexBufferStride = Backend::IndexBufferStride::Uint16;

		const auto lock = std::scoped_lock(m_Mutex);

		// TODO: The acceleration structures should be optimized.
		auto& drawData = m_DrawData.emplace_back(std::move(storage));
		drawData.m_pBottomLevelAccelerationStructure = m_Renderer.getInstance().getFactory()->createBottomLevelAccelerationStructure(m_Renderer.getInstance().getBackendDevice(), { geometry });
		drawData.m_pTopLevelAccelerationStructure = m_Renderer.getInstance().getFactory()->createTopLevelAccelerationStructure(m_Renderer.getInstance().getBackendDevice(), { drawData.m_pBottomLevelAccelerationStructure.get() });
		drawData.m_pPipeline = pPipeline;
	}
}