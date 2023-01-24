// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ShadowMapLayer.hpp"

#include "../Renderer.hpp"
#include "../DefaultCacheHandler.hpp"

#include "../../XenonShaderBank/ShadowMap/ShadowMap.vert.hpp"

#include <optick.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Xenon
{
	namespace Experimental
	{
		ShadowMapLayer::ShadowMapLayer(Renderer& renderer, Backend::Camera* pCamera, uint32_t priority /*= 4*/)
			: RasterizingLayer(renderer, priority, pCamera, Backend::AttachmentType::Depth)
		{
			// Create the pipeline.
			Backend::RasterizingPipelineSpecification specification = {};
			specification.m_VertexShader = Generated::CreateShaderShadowMap_vert();
			// specification.m_DepthBiasFactor = 1.25f;
			// specification.m_DepthSlopeFactor = 1.75f;

			m_pPipeline = m_Renderer.getInstance().getFactory()->createRasterizingPipeline(m_Renderer.getInstance().getBackendDevice(), std::make_unique<DefaultCacheHandler>(), m_pRasterizer.get(), specification);

			// Setup the other texture information.
			m_pImageView = m_Renderer.getInstance().getFactory()->createImageView(m_Renderer.getInstance().getBackendDevice(), getShadowImage(), {});
			m_pImageSampler = m_Renderer.getInstance().getFactory()->createImageSampler(m_Renderer.getInstance().getBackendDevice(), {});

			// Setup the light buffers and descriptors.
			m_LightCamera.m_pBuffer = m_Renderer.getInstance().getFactory()->createBuffer(m_Renderer.getInstance().getBackendDevice(), sizeof(ShadowCamera), Backend::BufferType::Uniform);
			m_LightCamera.m_pDescriptor = m_pPipeline->createDescriptor(Backend::DescriptorType::Scene);

			m_LightCamera.m_pDescriptor->attach(EnumToInt(Backend::SceneBindings::Camera), m_LightCamera.m_pBuffer.get());
		}

		void ShadowMapLayer::onUpdate(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex)
		{
			OPTICK_EVENT();

			// Begin recording.
			m_pCommandRecorder->begin();

			// Bind the render target.
			m_pCommandRecorder->bind(m_pRasterizer.get(), { 1.0f });

			// Set the scissor and view port.
			m_pCommandRecorder->setViewport(0.0f, 0.0f, static_cast<float>(m_Renderer.getCamera()->getWidth()), static_cast<float>(m_Renderer.getCamera()->getHeight()), 0.0f, 1.0f);
			m_pCommandRecorder->setScissor(0, 0, m_Renderer.getCamera()->getWidth(), m_Renderer.getCamera()->getHeight());

			// Issue the draw calls.
			issueDrawCalls();

			// End the command recorder recording.
			m_pCommandRecorder->end();
		}

		Xenon::Texture ShadowMapLayer::getShadowTexture()
		{
			Texture texture;
			texture.m_pImage = getShadowImage();
			texture.m_pImageView = m_pImageView.get();
			texture.m_pImageSampler = m_pImageSampler.get();

			return texture;
		}

		void ShadowMapLayer::issueDrawCalls()
		{
			OPTICK_EVENT();

			// Return without doing anything is a scene is not attached.
			if (m_pScene == nullptr)
				return;

			// Get the registry reference.
			auto& registry = m_pScene->getRegistry();

			// Setup the light sources.
			for (const auto& group : registry.view<Components::LightSource>())
			{
				m_LightCamera.m_Camera = calculateShadowCamera(registry.get<Components::LightSource>(group));
				m_LightCamera.m_pBuffer->write(ToBytes(&m_LightCamera.m_Camera), sizeof(ShadowCamera));

				// Break from the loop since we only need one.
				break;
			}

			// Registry any new materials.
			for (const auto& group : registry.view<Geometry, Material>())
			{
				// const auto& material = registry.get<Material>(group);
				// const auto& materialSpecification = m_Renderer.getInstance().getMaterialDatabase().getSpecification(material);

				// // Setup the per-geometry descriptor if we need one for the geometry.
				// if (!pipeline.m_pPerGeometryDescriptors.contains(group))
				// 	pipeline.m_pPerGeometryDescriptors[group] = createPerGeometryDescriptor(pipeline, group);
				// 
				// // Get the per-geometry descriptor.
				// auto pPerGeometryDescriptor = pipeline.m_pPerGeometryDescriptors[group].get();

				// Issue draw calls.
				auto& geometry = registry.get<Geometry>(group);

				// Geometry pass time!
				m_pCommandRecorder->bind(m_pPipeline.get(), geometry.getVertexSpecification());
				m_pCommandRecorder->bind(geometry.getVertexBuffer(), geometry.getVertexSpecification().getSize());

				// Bind the sub-meshes.
				for (const auto& mesh : geometry.getMeshes())
				{
					OPTICK_EVENT_DYNAMIC("Binding Mesh");

					for (const auto& subMesh : mesh.m_SubMeshes)
					{
						OPTICK_EVENT_DYNAMIC("Issuing Draw Calls");

						m_pCommandRecorder->bind(geometry.getIndexBuffer(), static_cast<Backend::IndexBufferStride>(subMesh.m_IndexSize));
						m_pCommandRecorder->bind(m_pPipeline.get(), nullptr, nullptr, nullptr, m_LightCamera.m_pDescriptor.get());

						m_pCommandRecorder->drawIndexed(subMesh.m_VertexOffset, subMesh.m_IndexOffset, subMesh.m_IndexCount);
					}
				}
			}
		}

		Xenon::Experimental::ShadowMapLayer::ShadowCamera ShadowMapLayer::calculateShadowCamera(const Components::LightSource& lightSource) const
		{
			OPTICK_EVENT();

			ShadowCamera camera = {};
			camera.m_View = glm::lookAt(lightSource.m_Position, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			camera.m_Projection = glm::perspective(glm::radians(lightSource.m_FieldAngle), 1.0f, m_Renderer.getCamera()->m_NearPlane, m_Renderer.getCamera()->m_FarPlane);

			return camera;
		}
	}
}