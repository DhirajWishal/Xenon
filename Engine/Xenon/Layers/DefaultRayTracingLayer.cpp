// Copyright 2022-2023 Dhiraj Wishal
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
			m_pCommandRecorder->bind(drawData.m_pPipeline);
			m_pCommandRecorder->bind(drawData.m_pPipeline, nullptr, nullptr, nullptr, nullptr);
			m_pCommandRecorder->drawRayTraced(m_pRayTracer.get(), drawData.m_pShaderBindingTable.get());
		}

		m_pCommandRecorder->end();
	}

	void DefaultRayTracingLayer::addDrawData(Geometry&& geometry, Backend::RayTracingPipeline* pPipeline)
	{
		OPTICK_EVENT();

		// Setup the acceleration structure geometry.
		Backend::AccelerationStructureGeometry ASGeometry;
		ASGeometry.m_VertexSpecification = geometry.getVertexSpecification();
		ASGeometry.m_pVertexBuffer = geometry.getVertexBuffer();
		ASGeometry.m_pIndexBuffer = geometry.getIndexBuffer();
		ASGeometry.m_IndexBufferStride = Backend::IndexBufferStride::Uint16;

		// Setup the shader binding table.
		Backend::ShaderBindingTableBuilder sbtBuilder = {};

		const auto lock = std::scoped_lock(m_Mutex);

		// TODO: The acceleration structures should be optimized.
		auto& drawData = m_DrawData.emplace_back(std::move(geometry));
		drawData.m_pBottomLevelAccelerationStructure = m_Renderer.getInstance().getFactory()->createBottomLevelAccelerationStructure(m_Renderer.getInstance().getBackendDevice(), { ASGeometry });
		drawData.m_pTopLevelAccelerationStructure = m_Renderer.getInstance().getFactory()->createTopLevelAccelerationStructure(m_Renderer.getInstance().getBackendDevice(), { drawData.m_pBottomLevelAccelerationStructure.get() });
		drawData.m_pPipeline = pPipeline;
		drawData.m_pShaderBindingTable = pPipeline->createShaderBindingTable(sbtBuilder.getBindingGroups());
	}
}