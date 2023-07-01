// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GBufferLayer.hpp"

#include "../Renderer.hpp"
#include "../DefaultCacheHandler.hpp"

#include "../../XenonShaderBank/GBuffer/GBuffer.vert.hpp"
#include "../../XenonShaderBank/GBuffer/GBuffer.frag.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <optick.h>

constexpr auto g_PositiveXFace = glm::vec3(0.0f, 0.0f, 1.0f);
constexpr auto g_NegativeXFace = glm::vec3(0.0f, 0.0f, -1.0f);
// constexpr auto g_PositiveXFace = glm::vec3(0.0f, 0.0f, -1.0f);
// constexpr auto g_NegativeXFace = glm::vec3(0.0f, 0.0f, 1.0f);
// constexpr auto g_PositiveXFace = glm::vec3(0.0f, 0.0f, -1.0f);
// constexpr auto g_NegativeXFace = glm::vec3(0.0f, 0.0f, 1.0f);

constexpr auto g_Resolution = 1000;

namespace Xenon
{
	namespace Experimental
	{
		GBufferLayer::GBufferLayer(Renderer& renderer, uint32_t width, uint32_t height, GBufferFace face /*= GBufferFace::Front*/, uint32_t priority /*= 0*/)
			: RasterizingLayer(renderer, priority, width, height, Backend::AttachmentType::Color | Backend::AttachmentType::Normal | Backend::AttachmentType::Position | Backend::AttachmentType::Depth)
			, m_pRotationBuffer(renderer.getInstance().getFactory()->createBuffer(renderer.getInstance().getBackendDevice(), sizeof(glm::mat4), Backend::BufferType::Uniform))
			, m_Face(face)
		{
			// Setup the rotation matrix for the required face.
			switch (face)
			{
			case Xenon::Experimental::GBufferFace::PositiveX:
				m_RotationMatrix = glm::rotate(m_RotationMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				m_RotationMatrix = glm::rotate(m_RotationMatrix, glm::radians(-180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				break;

			case Xenon::Experimental::GBufferFace::NegativeX:
				m_RotationMatrix = glm::rotate(m_RotationMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				m_RotationMatrix = glm::rotate(m_RotationMatrix, glm::radians(-180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				break;

			case Xenon::Experimental::GBufferFace::PositiveY:
				m_RotationMatrix = glm::rotate(m_RotationMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				break;

			case Xenon::Experimental::GBufferFace::NegativeY:
				m_RotationMatrix = glm::rotate(m_RotationMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				break;

			case Xenon::Experimental::GBufferFace::PositiveZ:
				m_RotationMatrix = glm::rotate(m_RotationMatrix, glm::radians(-180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				break;

			case Xenon::Experimental::GBufferFace::NegativeZ:
				// m_RotationMatrix = glm::rotate(m_RotationMatrix, glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				break;

			default:
				break;
			}

			// Create the pipeline.
			Backend::RasterizingPipelineSpecification specification = {};
			specification.m_VertexShader = Generated::CreateShaderGBuffer_vert();
			specification.m_FragmentShader = Generated::CreateShaderGBuffer_frag();
			specification.m_ColorBlendAttachments.resize(3);

			m_pPipeline = m_Renderer.getInstance().getFactory()->createRasterizingPipeline(m_Renderer.getInstance().getBackendDevice(), std::make_unique<DefaultCacheHandler>(), m_pRasterizer.get(), specification);

			// Setup the descriptors.
			m_pUserDefinedDescriptor = m_pPipeline->createDescriptor(Backend::DescriptorType::UserDefined);
			m_pUserDefinedDescriptor->attach(0, m_pRotationBuffer.get());

			m_pSceneDescriptor = m_pPipeline->createDescriptor(Backend::DescriptorType::Scene);

			// Setup the light image.
			Backend::ImageSpecification lightImageSpecification = {};
			lightImageSpecification.m_Width = g_Resolution;
			lightImageSpecification.m_Height = g_Resolution;
			lightImageSpecification.m_Type = Backend::ImageType::CubeMap; // Not sure about this bud...
			lightImageSpecification.m_Usage = Backend::ImageUsage::Storage | Backend::ImageUsage::Graphics;
			lightImageSpecification.m_Format = Backend::DataFormat::R8_UNORMAL;
			m_pLightImage = renderer.getInstance().getFactory()->createImage(renderer.getInstance().getBackendDevice(), lightImageSpecification);
		}

		void GBufferLayer::onPreUpdate()
		{
			OPTICK_EVENT();

			// Return if no scene is attached.
			if (!m_pScene)
				return;

			// Iterate over the geometries and setup the descriptors.
			for (const auto& group : m_pScene->getRegistry().view<Geometry, Material>())
			{
				for (auto& mesh : m_pScene->getRegistry().get<Geometry>(group).getMeshes())
				{
					for (auto& subMesh : mesh.m_SubMeshes)
						createMaterial(subMesh);
				}
			}
		}

		void GBufferLayer::onUpdate(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex)
		{
			OPTICK_EVENT();

			// Rotate the camera.
			rotateCamera();

			// Begin the command recorder.
			m_pCommandRecorder->begin();

			// Bind the render target.
			m_pCommandRecorder->bind(m_pRasterizer.get(), { glm::vec4(0.0f), glm::vec4(0.0f), glm::vec4(0.0f), 1.0f });

			// Set the scissor and view port.
			m_pCommandRecorder->setViewport(0.0f, 0.0f, static_cast<float>(m_Renderer.getWindow()->getWidth()), static_cast<float>(m_Renderer.getWindow()->getHeight()), 0.0f, 1.0f);
			m_pCommandRecorder->setScissor(0, 0, m_Renderer.getWindow()->getWidth(), m_Renderer.getWindow()->getHeight());

			// Issue the draw calls if we have a scene.
			if (m_pScene)
				issueDrawCalls();

			// End the command recorder.
			m_pCommandRecorder->end();
		}

		void GBufferLayer::setScene(Scene& scene)
		{
			OPTICK_EVENT();

			m_pScene = &scene;
			scene.setupDescriptor(m_pSceneDescriptor.get(), m_pPipeline.get());
		}

		void GBufferLayer::issueDrawCalls()
		{
			OPTICK_EVENT();

			// Iterate over the geometries and draw.
			for (const auto& group : m_pScene->getRegistry().view<Geometry, Material>())
			{
				// Issue draw calls.
				auto& geometry = m_pScene->getRegistry().get<Geometry>(group);

				// Geometry pass time!
				m_pCommandRecorder->bind(m_pPipeline.get(), geometry.getVertexSpecification());
				m_pCommandRecorder->bind(geometry.getVertexBuffer(), geometry.getVertexSpecification().getSize());

				// Bind the sub-meshes.
				for (const auto& mesh : geometry.getMeshes())
				{
					OPTICK_EVENT_DYNAMIC("Binding Mesh");

					for (const auto& subMesh : mesh.m_SubMeshes)
					{
						OPTICK_EVENT_DYNAMIC("Issuing Occlusion Pass Draw Calls");

						m_pCommandRecorder->bind(geometry.getIndexBuffer(), static_cast<Backend::IndexBufferStride>(subMesh.m_IndexSize));
						m_pCommandRecorder->bind(m_pPipeline.get(), m_pUserDefinedDescriptor.get(), m_pMaterialDescriptors[subMesh].get(), nullptr, m_pSceneDescriptor.get());

						m_pCommandRecorder->drawIndexed(subMesh.m_VertexOffset, subMesh.m_IndexOffset, subMesh.m_IndexCount);
					}
				}
			}
		}

		void GBufferLayer::createMaterial(SubMesh& subMesh)
		{
			OPTICK_EVENT();

			// Get the material if we already have one.
			if (m_pMaterialDescriptors.contains(subMesh))
				return;

			// Setup the new material descriptors.
			const auto& pDescriptors = m_pMaterialDescriptors[subMesh] = m_pPipeline->createDescriptor(Backend::DescriptorType::Material);
			pDescriptors->attach(0, subMesh.m_BaseColorTexture.m_pImage, subMesh.m_BaseColorTexture.m_pImageView, subMesh.m_BaseColorTexture.m_pImageSampler, Backend::ImageUsage::Graphics);
		}

		void GBufferLayer::rotateCamera()
		{
			OPTICK_EVENT();

			// Get the camera information.
			const auto position = m_pScene->getCamera()->m_Position;
			const auto cameraUp = m_pScene->getCamera()->m_Up;
			// const auto front = m_RotationMatrix * glm::vec4(m_Renderer.getCamera()->m_Front, 1.0f);
			
			// Calculate the view-model matrix.
			// const auto matrix = glm::lookAt(position, position + glm::vec3(front), cameraUp);
			const auto matrix = glm::lookAt(position, position + m_pScene->getCamera()->m_Front, cameraUp) * m_RotationMatrix;
			
			// Copy the rotation matrix.
			m_pRotationBuffer->write(ToBytes(glm::value_ptr(matrix)), sizeof(glm::mat4));
		}
	}
}