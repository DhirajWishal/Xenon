// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DefaultRasterizingLayer.hpp"

#include "../Renderer.hpp"
#include "../DefaultCacheHandler.hpp"

#include "../../XenonCore/Logging.hpp"

#include <optick.h>
#include <glm/vec4.hpp>

namespace Xenon
{
	DefaultRasterizingLayer::DefaultRasterizingLayer(Renderer& renderer, uint32_t width, uint32_t height, uint32_t priority/* = 5*/)
		: RasterizingLayer(renderer, priority, width, height, Backend::AttachmentType::Color | Backend::AttachmentType::Depth | Backend::AttachmentType::Stencil)
	{
	}

	void DefaultRasterizingLayer::onUpdate(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex)
	{
		OPTICK_EVENT();

		// Begin recording.
		m_pCommandRecorder->begin();

		// Bind the render target.
		m_pCommandRecorder->bind(m_pRasterizer.get(), { glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, static_cast<uint32_t>(0) });

		// Set the scissor and view port.
		m_pCommandRecorder->setViewport(0.0f, 0.0f, static_cast<float>(m_Renderer.getWindow()->getWidth()), static_cast<float>(m_Renderer.getWindow()->getHeight()), 0.0f, 1.0f);
		m_pCommandRecorder->setScissor(0, 0, m_Renderer.getWindow()->getWidth(), m_Renderer.getWindow()->getHeight());

		// Issue the draw calls.
		issueDrawCalls();

		// End the command recorder recording.
		m_pCommandRecorder->end();
	}

	std::unique_ptr<Xenon::Backend::Descriptor> DefaultRasterizingLayer::createPerGeometryDescriptor(Pipeline& pipeline, Group group)
	{
		OPTICK_EVENT();

		std::unique_ptr<Xenon::Backend::Descriptor> pDescriptor = pipeline.m_pPipeline->createDescriptor(Backend::DescriptorType::PerGeometry);
		if (m_pScene->getRegistry().any_of<Components::Transform>(group))
			pDescriptor->attach(EnumToInt(Backend::PerGeometryBindings::Transform), m_pScene->getRegistry().get<Internal::TransformUniformBuffer>(group).m_pUniformBuffer.get());

		return pDescriptor;
	}

	void DefaultRasterizingLayer::setupMaterialDescriptor(Pipeline& pipeline, SubMesh& subMesh, const MaterialSpecification& specification) const
	{
		OPTICK_EVENT();

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

			case MaterialPropertyType::ShadowMap:
			{
				const auto& texture = std::get<0>(payload);
				pDescriptor->attach(binding, texture.m_pImage, texture.m_pImageView, texture.m_pImageSampler, Backend::ImageUsage::Graphics);

				break;
			}

			case MaterialPropertyType::Custom:
			{
				if (payload.index() == 0)
				{
					const auto& texture = std::get<0>(payload);
					pDescriptor->attach(binding, texture.m_pImage, texture.m_pImageView, texture.m_pImageSampler, Backend::ImageUsage::Graphics);
				}
				else
				{
					const auto& pBuffer = std::get<1>(payload);
					pDescriptor->attach(binding, pBuffer);
				}

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

		// Reset the counters.
		m_DrawCount = 0;

		// Registry any new materials.
		for (const auto& group : m_pScene->getRegistry().view<Geometry, Material>())
		{
			const auto& material = m_pScene->getRegistry().get<Material>(group);
			const auto& materialSpecification = m_Renderer.getInstance().getMaterialDatabase().getSpecification(material);

			// Setup the material's pipeline if we need to.
			if (!m_pPipelines.contains(material))
			{
				OPTICK_EVENT_DYNAMIC("Creating Pipeline For Material");

				// Create the pipeline.
				auto& pipeline = m_pPipelines[material];
				pipeline.m_pPipeline = m_Renderer.getInstance().getFactory()->createRasterizingPipeline(
					m_Renderer.getInstance().getBackendDevice(),
					std::make_unique<DefaultCacheHandler>(),
					m_pRasterizer.get(),
					materialSpecification.m_RasterizingPipelineSpecification
				);

				// Create the camera descriptor.
				pipeline.m_pSceneDescriptor = pipeline.m_pPipeline->createDescriptor(Backend::DescriptorType::Scene);
				m_pScene->setupDescriptor(pipeline.m_pSceneDescriptor.get(), pipeline.m_pPipeline.get());
			}

			// Get the pipeline.
			auto& pipeline = m_pPipelines[material];

			// Setup the per-geometry descriptor if we need one for the geometry.
			if (!pipeline.m_pPerGeometryDescriptors.contains(group))
				pipeline.m_pPerGeometryDescriptors[group] = createPerGeometryDescriptor(pipeline, group);

			// Get the per-geometry descriptor.
			auto pPerGeometryDescriptor = pipeline.m_pPerGeometryDescriptors[group].get();

			// Issue draw calls.
			auto& geometry = m_pScene->getRegistry().get<Geometry>(group);

			// Setup the material descriptors if we need to.
			for (auto& mesh : geometry.getMeshes())
			{
				for (auto& subMesh : mesh.m_SubMeshes)
					setupMaterialDescriptor(pipeline, subMesh, materialSpecification);
			}

			// Geometry pass time!
			geometryPass(pPerGeometryDescriptor, geometry, pipeline);
		}
	}

	void DefaultRasterizingLayer::geometryPass(Backend::Descriptor* pPerGeometryDescriptor, Geometry& geometry, Pipeline& pipeline)
	{
		OPTICK_EVENT();

		m_pCommandRecorder->bind(pipeline.m_pPipeline.get(), geometry.getVertexSpecification());
		m_pCommandRecorder->bind(geometry.getVertexBuffer(), geometry.getVertexSpecification().getSize());

		// Bind the sub-meshes.
		for (const auto& mesh : geometry.getMeshes())
		{
			OPTICK_EVENT_DYNAMIC("Binding Mesh");

			for (const auto& subMesh : mesh.m_SubMeshes)
			{
				OPTICK_EVENT_DYNAMIC("Issuing Draw Calls");

				// If the sub-mesh is occluded just skip.
				if (m_pOcclusionLayer && m_pOcclusionLayer->getSamples(subMesh) == 0)
					continue;

				m_pCommandRecorder->bind(pipeline.m_pPipeline.get(), nullptr, pipeline.m_pMaterialDescriptors[subMesh].get(), pPerGeometryDescriptor, pipeline.m_pSceneDescriptor.get());

				if (subMesh.m_IndexCount > 0)
				{
					m_pCommandRecorder->bind(geometry.getIndexBuffer(), static_cast<Backend::IndexBufferStride>(subMesh.m_IndexSize));
					m_pCommandRecorder->drawIndexed(subMesh.m_VertexOffset, subMesh.m_IndexOffset, subMesh.m_IndexCount);
				}
				else
				{
					m_pCommandRecorder->drawVertices(subMesh.m_VertexOffset, subMesh.m_VertexCount);
				}

				m_DrawCount++;
			}
		}
	}
}