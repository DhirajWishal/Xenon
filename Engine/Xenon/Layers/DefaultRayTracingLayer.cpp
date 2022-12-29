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
		m_pCommandRecorder->begin();
		m_pCommandRecorder->end();
	}

	void DefaultRayTracingLayer::addDrawData(MeshStorage&& storage/*, Backend::RasterizingPipeline* pPipeline*/)
	{
		Backend::AccelerationStructureGeometry geometry;
		geometry.m_VertexSpecification = storage.getVertexSpecification();
		geometry.m_pVertexBuffer = storage.getVertexBuffer();
		geometry.m_pIndexBuffer = storage.getIndexBuffer();
		geometry.m_IndexBufferStride = Backend::IndexBufferStride::Uint16;

		auto pBottomLevelAS = m_Renderer.getInstance().getFactory()->createBottomLevelAccelerationStructure(m_Renderer.getInstance().getBackendDevice(), { geometry });
		auto pTopLevelAS = m_Renderer.getInstance().getFactory()->createTopLevelAccelerationStructure(m_Renderer.getInstance().getBackendDevice(), { pBottomLevelAS.get() });
	}
}