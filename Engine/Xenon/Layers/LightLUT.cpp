// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "LightLUT.hpp"
#include "DirectLightingLayer.hpp"

#include "../Renderer.hpp"
#include "../DefaultCacheHandler.hpp"

#include "../../XenonShaderBank/LightLUT/LightLUT.vert.hpp"
#include "../../XenonShaderBank/LightLUT/LightLUT.frag.hpp"

#include <optick.h>

namespace Xenon
{
	namespace Experimental
	{
		LightLUT::LightLUT(Renderer& renderer, Backend::Camera* pCamera, uint32_t priority)
			: RasterizingLayer(renderer, priority, pCamera, Backend::AttachmentType::Color)
			, m_pLookUpTable(renderer.getInstance().getFactory()->createBuffer(renderer.getInstance().getBackendDevice(), sizeof(glm::vec2), Backend::BufferType::Storage))
			, m_pControlBlock(renderer.getInstance().getFactory()->createBuffer(renderer.getInstance().getBackendDevice(), sizeof(ControlBlock), Backend::BufferType::Uniform))
		{
			// Create the pipeline.
			Backend::RasterizingPipelineSpecification specification = {};
			specification.m_VertexShader = Generated::CreateShaderLightLUT_vert();
			specification.m_FragmentShader = Generated::CreateShaderLightLUT_frag();
			specification.m_CullMode = Backend::CullMode::None;
			specification.m_PolygonMode = Backend::PolygonMode::Point;

			m_pPipeline = m_Renderer.getInstance().getFactory()->createRasterizingPipeline(m_Renderer.getInstance().getBackendDevice(), std::make_unique<DefaultCacheHandler>(), m_pRasterizer.get(), specification);

			// Setup the descriptors.
			m_pUserDefinedDescriptor = m_pPipeline->createDescriptor(Backend::DescriptorType::UserDefined);
			m_pSceneDescriptor = m_pPipeline->createDescriptor(Backend::DescriptorType::Scene);

			// Set the default attachments.
			m_pUserDefinedDescriptor->attach(0, m_pControlBlock.get());
			m_pUserDefinedDescriptor->attach(1, m_pLookUpTable.get());

			// Copy the initial data.
			m_pControlBlock->writeObject(m_ControlBlock);
		}

		void LightLUT::onPreUpdate()
		{
			OPTICK_EVENT();

			// Get the light count.
			uint32_t lightCount = 0;
			for ([[maybe_unused]] const auto& group : m_pScene->getRegistry().view<Components::LightSource>())
				lightCount++;

			// Get the vertex count.
			uint64_t vertexCount = 0;
			for (const auto& group : m_pScene->getRegistry().view<Geometry, Material>())
			{
				const auto& geometry = m_pScene->getRegistry().get<Geometry>(group);
				vertexCount += (geometry.getVertexBuffer()->getSize() / geometry.getVertexSpecification().getSize());
			}

			// Setup the buffers again if needed.
			const auto requriedBufferSize = vertexCount * lightCount * sizeof(glm::vec2);
			if (requriedBufferSize > 0 && requriedBufferSize != m_pLookUpTable->getSize())
			{
				m_Renderer.getInstance().getBackendDevice()->waitIdle();
				m_pLookUpTable = m_Renderer.getInstance().getFactory()->createBuffer(m_Renderer.getInstance().getBackendDevice(), std::max(requriedBufferSize, sizeof(glm::vec2)), Backend::BufferType::Storage);
				m_pUserDefinedDescriptor->attach(1, m_pLookUpTable.get());
				m_pAttachment->setLightLUT(this);

				m_ControlBlock.m_Stride = static_cast<uint32_t>(vertexCount);
				m_pControlBlock->writeObject(m_ControlBlock);
			}
		}

		void LightLUT::onUpdate(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex)
		{
			OPTICK_EVENT();

			// Begin recording.
			m_pCommandRecorder->begin();

			// Bind the render target.
			m_pCommandRecorder->bind(m_pRasterizer.get(), { glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) });

			// Setup and render everything only if we have an active scene.
			if (m_pScene)
			{
				// Set the scissor and view port.
				m_pCommandRecorder->setViewport(0.0f, 0.0f, static_cast<float>(m_Renderer.getCamera()->getWidth()), static_cast<float>(m_Renderer.getCamera()->getHeight()), 0.0f, 1.0f);
				m_pCommandRecorder->setScissor(0, 0, m_Renderer.getCamera()->getWidth(), m_Renderer.getCamera()->getHeight());

				// Issue the draw calls.
				issueDrawCalls();
			}

			// End the command recorder recording.
			m_pCommandRecorder->end();
		}

		void LightLUT::setScene(Scene& scene)
		{
			OPTICK_EVENT();

			m_pScene = &scene;
			m_pScene->setupDescriptor(m_pSceneDescriptor.get(), m_pPipeline.get());
		}

		void LightLUT::setAttachment(DirectLightingLayer* pLayer)
		{
			OPTICK_EVENT();

			m_pAttachment = pLayer;
		}

		void LightLUT::issueDrawCalls()
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
						m_pCommandRecorder->bind(m_pPipeline.get(), m_pUserDefinedDescriptor.get(), nullptr, nullptr, m_pSceneDescriptor.get());

						m_pCommandRecorder->drawIndexed(subMesh.m_VertexOffset, subMesh.m_IndexOffset, subMesh.m_IndexCount);
					}
				}
			}
		}
	}
}