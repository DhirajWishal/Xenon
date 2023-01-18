// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "OcclusionLayer.hpp"
#include "../Renderer.hpp"

#include "../../XenonShaderBank/Occlusion/Occlusion.vert.hpp"
#include "../../XenonShaderBank/Occlusion/Occlusion.frag.hpp"

#include <optick.h>

namespace Xenon
{
	OcclusionLayer::OcclusionLayer(Renderer& renderer, Backend::Camera* pCamera, uint32_t priority /*= 5*/)
		: RasterizingLayer(renderer, priority, pCamera, Backend::AttachmentType::Depth | Backend::AttachmentType::Stencil)
		, m_pOcclusionQuery(renderer.getInstance().getFactory()->createOcclusionQuery(m_Renderer.getInstance().getBackendDevice(), 1))
	{
		// Create the pipeline.
		Backend::RasterizingPipelineSpecification specification = {};
		specification.m_VertexShader = Generated::CreateShaderOcclusion_vert();
		// specification.m_FragmentShader = Generated::CreateShaderOcclusion_frag();

		m_pOcclusionPipeline = m_Renderer.getInstance().getFactory()->createRasterizingPipeline(m_Renderer.getInstance().getBackendDevice(), nullptr, m_pRasterizer.get(), specification);

		// Setup the occlusion camera descriptor.
		m_pOcclusionSceneDescriptor = m_pOcclusionPipeline->createDescriptor(Backend::DescriptorType::Scene);
		m_pOcclusionSceneDescriptor->attach(0, m_Renderer.getCamera()->getViewports().front().m_pUniformBuffer);
	}

	void OcclusionLayer::onUpdate(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex)
	{
		OPTICK_EVENT();

		// Begin recording.
		m_pCommandRecorder->begin();

		// Get the drawable count.
		const auto subMeshCount = m_pScene->getDrawableCount();

		// Re-create the occlusion query if needed.
		{
			auto lock = std::scoped_lock(m_Mutex);
			if (subMeshCount > 0 && m_pOcclusionQuery->getSampleCount() != subMeshCount)
			{
				m_Renderer.getInstance().getBackendDevice()->waitIdle();
				m_pOcclusionQuery = m_Renderer.getInstance().getFactory()->createOcclusionQuery(m_Renderer.getInstance().getBackendDevice(), subMeshCount);
			}
		}

#ifdef ENABLE_OCCLUSION_CULL
		// Reset the query.
		m_pCommandRecorder->resetQuery(m_pOcclusionQuery.get());

#endif // ENABLE_OCCLUSION_CULL

		// Bind the render target.
		m_pCommandRecorder->bind(m_pRasterizer.get(), { 1.0f, static_cast<uint32_t>(0) });

		// Set the scissor and view port.
		m_pCommandRecorder->setViewport(0.0f, 0.0f, static_cast<float>(m_Renderer.getCamera()->getWidth()), static_cast<float>(m_Renderer.getCamera()->getHeight()), 0.0f, 1.0f);
		m_pCommandRecorder->setScissor(0, 0, m_Renderer.getCamera()->getWidth(), m_Renderer.getCamera()->getHeight());

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
					m_pCommandRecorder->bind(m_pOcclusionPipeline.get(), nullptr, nullptr, pPerGeometryDescriptor, m_pOcclusionSceneDescriptor.get());

					m_pCommandRecorder->beginQuery(m_pOcclusionQuery.get(), static_cast<uint32_t>(0));
					m_pCommandRecorder->drawIndexed(subMesh.m_VertexOffset, subMesh.m_IndexOffset, subMesh.m_IndexCount);
					m_pCommandRecorder->endQuery(m_pOcclusionQuery.get(), static_cast<uint32_t>(0));
				}
			}
		}

		// Query the results only if we have drawn something.
		if (subMeshCount > 0)
			m_pCommandRecorder->getQueryResults(m_pOcclusionQuery.get());

		// End the command recorder recording.
		m_pCommandRecorder->end();
	}

	std::unique_ptr<Xenon::Backend::Descriptor> OcclusionLayer::createPerGeometryDescriptor(Group group)
	{
		OPTICK_EVENT();

		std::unique_ptr<Xenon::Backend::Descriptor> pDescriptor = m_pOcclusionPipeline->createDescriptor(Backend::DescriptorType::PerGeometry);
		if (m_pScene->getRegistry().any_of<Components::Transform>(group))
			pDescriptor->attach(EnumToInt(Backend::PerGeometryBindings::Transform), m_pScene->getRegistry().get<Internal::TransformUniformBuffer>(group).m_pUniformBuffer.get());

		return pDescriptor;
	}
}