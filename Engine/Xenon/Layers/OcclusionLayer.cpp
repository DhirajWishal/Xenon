// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#include "OcclusionLayer.hpp"

#include "../Renderer.hpp"
#include "../DefaultCacheHandler.hpp"

#include "../../XenonCore/Logging.hpp"

#include "../../XenonShaderBank/Occlusion/Occlusion.vert.hpp"

#include <optick.h>

namespace Xenon
{
	OcclusionLayer::OcclusionLayer(Renderer& renderer, uint32_t width, uint32_t height, uint32_t priority /*= 5*/)
		: RasterizingLayer(renderer, priority, width, height, Backend::AttachmentType::Depth | Backend::AttachmentType::Stencil)
	{
		// Create the pipeline.
		Backend::RasterizingPipelineSpecification specification = {};
		specification.m_VertexShader = Generated::CreateShaderOcclusion_vert();

		m_pOcclusionPipeline = m_Renderer.getInstance().getFactory()->createRasterizingPipeline(m_Renderer.getInstance().getBackendDevice(), std::make_unique<DefaultCacheHandler>(), m_pRasterizer.get(), specification);

		// Setup the occlusion query samples.
		m_OcclusionQuerySamples.reserve(m_pCommandRecorder->getBufferCount());
		for (uint32_t i = 0; i < m_pCommandRecorder->getBufferCount(); i++)
			m_OcclusionQuerySamples.emplace_back().m_pOcclusionQuery = m_Renderer.getInstance().getFactory()->createOcclusionQuery(m_Renderer.getInstance().getBackendDevice(), 1);
	}

	void OcclusionLayer::onPreUpdate()
	{
		OPTICK_EVENT();

		// Get the query samples structure for the current command buffer.
		auto& querySample = m_OcclusionQuerySamples[m_pCommandRecorder->getCurrentIndex()];

		if (m_pScene)
		{
			// Get the drawable count.
			const auto subMeshCount = m_pScene->getDrawableCount();

			// Re-create the occlusion query if needed.
			if (subMeshCount > 0 && querySample.m_pOcclusionQuery->getSampleCount() != subMeshCount)
			{
				m_Renderer.getInstance().getBackendDevice()->waitIdle();
				querySample.m_pOcclusionQuery = m_Renderer.getInstance().getFactory()->createOcclusionQuery(m_Renderer.getInstance().getBackendDevice(), subMeshCount);
				querySample.m_bHasQueryData = false;
			}
		}

		// If we have query data, get them and setup the samples map.
		if (querySample.m_bHasQueryData)
		{
			querySample.m_Samples = querySample.m_pOcclusionQuery->getSamples();

			for (const auto& [subMesh, index] : querySample.m_SubMeshIndexMap)
				querySample.m_SubMeshSamples[subMesh] = querySample.m_Samples[index];
		}
	}

	void OcclusionLayer::onUpdate(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex)
	{
		OPTICK_EVENT();

		// Begin recording.
		m_pCommandRecorder->begin();

		uint64_t subMeshCount = 0;
		auto& querySample = m_OcclusionQuerySamples[m_pCommandRecorder->getCurrentIndex()];

		// Reset the occlusion query. This must be done before binding the render target!
		if (m_pScene)
		{
			// Get the drawable count.
			subMeshCount = m_pScene->getDrawableCount();

			// Reset the query.
			m_pCommandRecorder->resetQuery(querySample.m_pOcclusionQuery.get());
			querySample.m_bHasQueryData = false;
		}

		// Bind the render target.
		m_pCommandRecorder->bind(m_pRasterizer.get(), { 1.0f, static_cast<uint32_t>(0) });

		// Draw if we have a scene attached.
		if (m_pScene)
			issueDrawCalls();

		// Query the results only if we have drawn something.
		if (subMeshCount > 0)
		{
			auto lock = std::scoped_lock(m_Mutex);
			m_pCommandRecorder->getQueryResults(querySample.m_pOcclusionQuery.get());
			querySample.m_bHasQueryData = true;
		}

		// End the command recorder recording.
		m_pCommandRecorder->end();
	}

	uint64_t OcclusionLayer::getSamples(const SubMesh& subMesh)
	{
		OPTICK_EVENT();

		auto lock = std::scoped_lock(m_Mutex);
		return m_OcclusionQuerySamples[m_pCommandRecorder->getCurrentIndex()].m_SubMeshSamples[subMesh];
	}

	void OcclusionLayer::issueDrawCalls()
	{
		OPTICK_EVENT();

		// Setup the occlusion scene descriptor if needed.
		if (!m_pOcclusionSceneDescriptors.contains(m_pScene))
		{
			const auto& pOcclusionSceneDescriptor = m_pOcclusionSceneDescriptors[m_pScene] = m_pOcclusionPipeline->createDescriptor(Backend::DescriptorType::Scene);
			m_pScene->setupDescriptor(pOcclusionSceneDescriptor.get(), m_pOcclusionPipeline.get());
		}

		// Get the scene descriptor.
		auto pOcclusionSceneDescriptor = m_pOcclusionSceneDescriptors[m_pScene].get();

		// Set the scissor and view port.
		m_pCommandRecorder->setViewport(0.0f, 0.0f, static_cast<float>(m_Renderer.getWindow()->getWidth()), static_cast<float>(m_Renderer.getWindow()->getHeight()), 0.0f, 1.0f);
		m_pCommandRecorder->setScissor(0, 0, m_Renderer.getWindow()->getWidth(), m_Renderer.getWindow()->getHeight());

		// Get the query samples structure for the current command buffer.
		auto& querySample = m_OcclusionQuerySamples[m_pCommandRecorder->getCurrentIndex()];

		uint32_t index = 0;
		for (const auto& group : m_pScene->getRegistry().view<Geometry, Material>())
		{
			// Setup the per-geometry descriptor if we need one for the geometry.
			if (!m_pPerGeometryDescriptors.contains(group))
				m_pPerGeometryDescriptors[group] = createPerGeometryDescriptor(group);

			// Get the per-geometry descriptor.
			auto pPerGeometryDescriptor = m_pPerGeometryDescriptors[group].get();

			// Issue draw calls.
			auto& geometry = m_pScene->getRegistry().get<Geometry>(group);

			// Occlusion pass time!
			m_pCommandRecorder->bind(m_pOcclusionPipeline.get(), geometry.getVertexSpecification());
			m_pCommandRecorder->bind(geometry.getVertexBuffer(), geometry.getVertexSpecification().getSize());

			// Bind the sub-meshes.
			for (const auto& mesh : geometry.getMeshes())
			{
				OPTICK_EVENT_DYNAMIC("Binding Mesh");

				for (const auto& subMesh : mesh.m_SubMeshes)
				{
					OPTICK_EVENT_DYNAMIC("Issuing Occlusion Pass Draw Calls");

					m_pCommandRecorder->bind(geometry.getIndexBuffer(), static_cast<Backend::IndexBufferStride>(subMesh.m_IndexSize));
					m_pCommandRecorder->bind(m_pOcclusionPipeline.get(), nullptr, nullptr, pPerGeometryDescriptor, pOcclusionSceneDescriptor);

					const auto subMeshIndex = querySample.m_SubMeshIndexMap[subMesh] = index;
					m_pCommandRecorder->beginQuery(querySample.m_pOcclusionQuery.get(), subMeshIndex);
					m_pCommandRecorder->drawIndexed(subMesh.m_VertexOffset, subMesh.m_IndexOffset, subMesh.m_IndexCount);
					m_pCommandRecorder->endQuery(querySample.m_pOcclusionQuery.get(), subMeshIndex);

					index++;
				}
			}
		}
	}

	std::unique_ptr<Xenon::Backend::Descriptor> OcclusionLayer::createPerGeometryDescriptor(Group group)
	{
		OPTICK_EVENT();

		// Create only if we need one of them. Else just don't...
		// if (m_pScene->getRegistry().any_of<Components::Transform>(group))
		// {
		// 	auto pDescriptor = m_pOcclusionPipeline->createDescriptor(Backend::DescriptorType::PerGeometry);
		// 	pDescriptor->attach(EnumToInt(Backend::PerGeometryBindings::Transform), m_pScene->getRegistry().get<Internal::TransformUniformBuffer>(group).m_pUniformBuffer.get());
		// 
		// 	return pDescriptor;
		// }

		return nullptr;
	}
}