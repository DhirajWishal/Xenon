// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DefaultRasterizingLayer.hpp"
#include "../Renderer.hpp"
#include "../../XenonCore/Logging.hpp"

#include <optick.h>
#include <glm/vec4.hpp>

// #define ENABLE_OCCLUSION_CULL

namespace Xenon
{
	DefaultRasterizingLayer::DefaultRasterizingLayer(Renderer& renderer, Backend::Camera* pCamera)
		: RasterizingLayer(renderer, pCamera, Backend::AttachmentType::Color | Backend::AttachmentType::Depth | Backend::AttachmentType::Stencil)
		, m_pOcclusionQuery(renderer.getInstance().getFactory()->createOcclusionQuery(m_Renderer.getInstance().getBackendDevice(), 1))
	{
		// Setup the occlusion pipeline.
		setupOcclusionPipeline();
	}

	void DefaultRasterizingLayer::onUpdate(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex)
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
		m_pCommandRecorder->bind(m_pRasterizer.get(), { glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, static_cast<uint32_t>(0) }, true);

		// Issue the draw calls.
		issueDrawCalls();

#ifdef ENABLE_OCCLUSION_CULL
		// Query the results only if we have drawn something.
		if (subMeshCount > 0)
			m_pCommandRecorder->getQueryResults(m_pOcclusionQuery.get());

#endif // ENABLE_OCCLUSION_CULL

		// End the command recorder recording.
		m_pCommandRecorder->end();
	}

	void DefaultRasterizingLayer::onRegisterCommandBuffers(std::vector<Backend::CommandRecorder*>& pCommandBuffers)
	{
		for (const auto& [material, pipeline] : m_pPipelines)
			pCommandBuffers.emplace_back(pipeline.m_pSecondaryCommandRecorder.get());
	}

	void DefaultRasterizingLayer::setupOcclusionPipeline()
	{
		OPTICK_EVENT();

#ifdef ENABLE_OCCLUSION_CULL
		// Create the pipeline.
		Backend::RasterizingPipelineSpecification specification = {};
		specification.m_VertexShader = Backend::ShaderSource::FromFile(XENON_SHADER_DIR "Occlusion/Shader.vert.spv");
		// specification.m_FragmentShader = Backend::ShaderSource::FromFile(XENON_SHADER_DIR "Occlusion/Shader.frag.spv");

		m_pOcclusionPipeline = m_Renderer.getInstance().getFactory()->createRasterizingPipeline(m_Renderer.getInstance().getBackendDevice(), nullptr, m_pRasterizer.get(), specification);

		// Setup the occlusion camera descriptor.
		m_pOcclusionCameraDescriptor = m_pOcclusionPipeline->createDescriptor(Backend::DescriptorType::Camera);
		m_pOcclusionCameraDescriptor->attach(0, m_Renderer.getCamera()->getViewports().front().m_pUniformBuffer);

#endif // ENABLE_OCCLUSION_CULL
	}

	std::unique_ptr<Xenon::Backend::Descriptor> DefaultRasterizingLayer::createPerGeometryDescriptor(Pipeline& pipeline, Group group)
	{
		OPTICK_EVENT();

		std::unique_ptr<Xenon::Backend::Descriptor> pDescriptor = pipeline.m_pPipeline->createDescriptor(Backend::DescriptorType::PerGeometry);
		if (m_pScene->getRegistry().any_of<Components::Transform>(group))
		{
			pDescriptor->attach(EnumToInt(Backend::PerGeometryBindings::Transform), m_pScene->getRegistry().get<Internal::TransformUniformBuffer>(group).m_pUniformBuffer.get());
		}

		return pDescriptor;
	}

	void DefaultRasterizingLayer::setupMaterialDescriptor(Pipeline& pipeline, SubMesh& subMesh, const MaterialSpecification& specification)
	{
		OPTICK_EVENT();

		auto lock = std::scoped_lock(m_Mutex);

		// Get if we've already crated a material descriptor for the sub-mesh.
		if (pipeline.m_pMaterialDescriptors.contains(subMesh))
			return;

		// Else let's quickly create one.
		uint32_t binding = 0;
		const auto& pDescriptor = pipeline.m_pMaterialDescriptors[subMesh] = pipeline.m_pPipeline->createDescriptor(Backend::DescriptorType::Material);
		for (const auto& [payload, type] : specification.m_Properties)
		{
			constexpr Texture useSubMeshTexture = {};

			switch (type)
			{
			case MaterialPropertyType::BaseColorTexture:
			{
				const auto& texture = std::get<0>(payload);
				if (texture == useSubMeshTexture)
					pDescriptor->attach(binding, subMesh.m_BaseColorTexture.m_pImage, subMesh.m_BaseColorTexture.m_pImageView, subMesh.m_BaseColorTexture.m_pImageSampler, Backend::ImageUsage::Graphics);

				else
					pDescriptor->attach(binding, texture.m_pImage, texture.m_pImageView, texture.m_pImageSampler, Backend::ImageUsage::Graphics);

				break;
			}

			case MaterialPropertyType::RoughnessTexture:
			{
				const auto& texture = std::get<0>(payload);
				if (texture == useSubMeshTexture)
					pDescriptor->attach(binding, subMesh.m_RoughnessTexture.m_pImage, subMesh.m_RoughnessTexture.m_pImageView, subMesh.m_RoughnessTexture.m_pImageSampler, Backend::ImageUsage::Graphics);

				else
					pDescriptor->attach(binding, texture.m_pImage, texture.m_pImageView, texture.m_pImageSampler, Backend::ImageUsage::Graphics);

				break;
			}

			case MaterialPropertyType::NormalTexture:
			{
				const auto& texture = std::get<0>(payload);
				if (texture == useSubMeshTexture)
					pDescriptor->attach(binding, subMesh.m_NormalTexture.m_pImage, subMesh.m_NormalTexture.m_pImageView, subMesh.m_NormalTexture.m_pImageSampler, Backend::ImageUsage::Graphics);

				else
					pDescriptor->attach(binding, texture.m_pImage, texture.m_pImageView, texture.m_pImageSampler, Backend::ImageUsage::Graphics);

				break;
			}

			case MaterialPropertyType::OcclusionTexture:
			{
				const auto& texture = std::get<0>(payload);
				if (texture == useSubMeshTexture)
					pDescriptor->attach(binding, subMesh.m_OcclusionTexture.m_pImage, subMesh.m_OcclusionTexture.m_pImageView, subMesh.m_OcclusionTexture.m_pImageSampler, Backend::ImageUsage::Graphics);

				else
					pDescriptor->attach(binding, texture.m_pImage, texture.m_pImageView, texture.m_pImageSampler, Backend::ImageUsage::Graphics);

				break;
			}

			case MaterialPropertyType::EmissiveTexture:
			{
				const auto& texture = std::get<0>(payload);
				if (texture == useSubMeshTexture)
					pDescriptor->attach(binding, subMesh.m_EmissiveTexture.m_pImage, subMesh.m_EmissiveTexture.m_pImageView, subMesh.m_EmissiveTexture.m_pImageSampler, Backend::ImageUsage::Graphics);

				else
					pDescriptor->attach(binding, texture.m_pImage, texture.m_pImageView, texture.m_pImageSampler, Backend::ImageUsage::Graphics);

				break;
			}

			default:
				break;
			}

			binding++;
		}
	}

	void DefaultRasterizingLayer::issueDrawCalls()
	{
		OPTICK_EVENT();

		// Return without doing anything is a scene is not attached.
		if (m_pScene == nullptr)
			return;

		// Lock the scene!
		auto lock = std::scoped_lock(m_pScene->getMutex());

		// Reset the counters.
		m_DrawCount = 0;

		// Return if we have nothing to draw.
		if (m_pScene->getDrawableGeometryCount() == 0)
			return;

		// Registry any new materials.
		for (const auto& group : m_pScene->getRegistry().view<Geometry, Material>())
		{
			const auto& material = m_pScene->getRegistry().get<Material>(group);
			const auto& materialSpecification = m_Renderer.getInstance().getMaterialDatabase().getSpecification(material);

			if (!m_pPipelines.contains(material))
			{
				OPTICK_EVENT_DYNAMIC("Creating Pipeline For Material");

				auto& pipeline = m_pPipelines[material];

				pipeline.m_pPipeline = m_Renderer.getInstance().getFactory()->createRasterizingPipeline(
					m_Renderer.getInstance().getBackendDevice(),
					nullptr,
					m_pRasterizer.get(),
					materialSpecification.m_RasterizingPipelineSpecification
				);

				// Create the camera descriptor.
				pipeline.m_pSceneDescriptor = pipeline.m_pPipeline->createDescriptor(Backend::DescriptorType::Scene);
				m_pScene->setupDescriptor(pipeline.m_pSceneDescriptor.get(), pipeline.m_pPipeline.get());

				// Create the secondary command recorder.
				pipeline.m_pSecondaryCommandRecorder =
					m_Renderer.getInstance().getFactory()->createCommandRecorder(
						m_Renderer.getInstance().getBackendDevice(),
						Backend::CommandRecorderUsage::Secondary,
						m_pCommandRecorder->getBufferCount()
					);
			}

			auto& pipeline = m_pPipelines[material];

			// Append the group.
			pipeline.m_Groups.emplace_back(group);

			// Setup the per-geometry descriptor if we need one for the geometry.
			if (!pipeline.m_pPerGeometryDescriptors.contains(group))
				pipeline.m_pPerGeometryDescriptors[group] = createPerGeometryDescriptor(pipeline, group);
		}

		// Reset the synchronization primitive.
		m_Synchronization.reset(m_pPipelines.size());

		// Iterate over the pipelines and bind everything.
		for (auto& [material, pipeline] : m_pPipelines)
		{
			OPTICK_EVENT_DYNAMIC("Binding Geometry");
			GetJobSystem().insert([this, &material, &pipeline] { issueDrawCalls(material, pipeline); });
		}

		// Wait till all the work is done.
		m_Synchronization.wait();
	}

	void DefaultRasterizingLayer::issueDrawCalls(const Material& material, Pipeline& pipeline)
	{
		OPTICK_EVENT();

		const auto& pCommandRecorder = pipeline.m_pSecondaryCommandRecorder.get();

		// Begin the command recorder.
		pCommandRecorder->begin(m_pCommandRecorder.get());

		// Set the scissor and view port.
		if (m_Renderer.getInstance().getBackendType() == BackendType::Vulkan)
		{
			pCommandRecorder->setViewport(0.0f, 0.0f, static_cast<float>(m_Renderer.getCamera()->getWidth()), static_cast<float>(m_Renderer.getCamera()->getHeight()), 0.0f, 1.0f);
			pCommandRecorder->setScissor(0, 0, m_Renderer.getCamera()->getWidth(), m_Renderer.getCamera()->getHeight());
		}

		// Draw the geometries!
		for (const auto& [group, pPerGeometryDescriptor] : pipeline.m_pPerGeometryDescriptors)
		{
			auto& geometry = m_pScene->getRegistry().get<Geometry>(group);
			const auto& materialSpecification = m_Renderer.getInstance().getMaterialDatabase().getSpecification(material);

			// Setup the material descriptors if we need to.
			for (auto& mesh : geometry.getMeshes())
			{
				for (auto& subMesh : mesh.m_SubMeshes)
					setupMaterialDescriptor(pipeline, subMesh, materialSpecification);
			}

#ifdef ENABLE_OCCLUSION_CULL
			// Occlusion pass time!
			occlusionPass(pCommandRecorder, geometry);

#endif // ENABLE_OCCLUSION_CULL

			// Geometry pass time!
			geometryPass(pCommandRecorder, pPerGeometryDescriptor.get(), geometry, pipeline);
		}

		// End the command recorder.
		pCommandRecorder->end();

		// Execute the children.
		if (m_Renderer.getInstance().getBackendType() == BackendType::DirectX_12)
		{
			m_pCommandRecorder->setViewport(0.0f, 0.0f, static_cast<float>(m_Renderer.getCamera()->getWidth()), static_cast<float>(m_Renderer.getCamera()->getHeight()), 0.0f, 1.0f);
			m_pCommandRecorder->setScissor(0, 0, m_Renderer.getCamera()->getWidth(), m_Renderer.getCamera()->getHeight());
		}

		m_pCommandRecorder->executeChild(pCommandRecorder, pipeline.m_pPipeline.get());

		// Select the next command recorder.
		pCommandRecorder->next();

		// Notify the parent that we're done.
		m_Synchronization.arrive();
	}

	void DefaultRasterizingLayer::occlusionPass(Backend::CommandRecorder* pCommandRecorder, Geometry& geometry) const
	{
		OPTICK_EVENT();

		pCommandRecorder->bind(m_pOcclusionPipeline.get(), geometry.getVertexSpecification());
		pCommandRecorder->bind(geometry.getVertexBuffer(), geometry.getVertexSpecification().getSize());

		// Bind the sub-meshes.
		for (const auto& mesh : geometry.getMeshes())
		{
			OPTICK_EVENT_DYNAMIC("Binding Mesh");

			for (const auto& subMesh : mesh.m_SubMeshes)
			{
				OPTICK_EVENT_DYNAMIC("Issuing Occlusion Pass Draw Calls");

				pCommandRecorder->bind(geometry.getIndexBuffer(), static_cast<Backend::IndexBufferStride>(subMesh.m_IndexSize));
				pCommandRecorder->bind(m_pOcclusionPipeline.get(), nullptr, nullptr, nullptr, m_pOcclusionCameraDescriptor.get());

				pCommandRecorder->beginQuery(m_pOcclusionQuery.get(), static_cast<uint32_t>(0));
				pCommandRecorder->drawIndexed(subMesh.m_VertexOffset, subMesh.m_IndexOffset, subMesh.m_IndexCount);
				pCommandRecorder->endQuery(m_pOcclusionQuery.get(), static_cast<uint32_t>(0));
			}
		}
	}

	void DefaultRasterizingLayer::geometryPass(Backend::CommandRecorder* pCommandRecorder, Backend::Descriptor* pPerGeometryDescriptor, Geometry& geometry, Pipeline& pipeline)
	{
		OPTICK_EVENT();

		pCommandRecorder->bind(pipeline.m_pPipeline.get(), geometry.getVertexSpecification());
		pCommandRecorder->bind(geometry.getVertexBuffer(), geometry.getVertexSpecification().getSize());

		// Bind the sub-meshes.
		for (auto& mesh : geometry.getMeshes())
		{
			OPTICK_EVENT_DYNAMIC("Binding Mesh");

			for (auto& subMesh : mesh.m_SubMeshes)
			{
				OPTICK_EVENT_DYNAMIC("Issuing Draw Calls");

				pCommandRecorder->bind(geometry.getIndexBuffer(), static_cast<Backend::IndexBufferStride>(subMesh.m_IndexSize));
				pCommandRecorder->bind(pipeline.m_pPipeline.get(), nullptr, pipeline.m_pMaterialDescriptors[subMesh].get(), pPerGeometryDescriptor, pipeline.m_pSceneDescriptor.get());

				pCommandRecorder->drawIndexed(subMesh.m_VertexOffset, subMesh.m_IndexOffset, subMesh.m_IndexCount);

				m_DrawCount++;
			}
		}
	}
}