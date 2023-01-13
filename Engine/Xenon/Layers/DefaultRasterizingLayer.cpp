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

	void DefaultRasterizingLayer::setupOcclusionPipeline()
	{
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

	Xenon::Backend::Descriptor* DefaultRasterizingLayer::getMaterialDescriptor(Pipeline& pipeline, SubMesh& subMesh, const MaterialSpecification& specification)
	{
		// Get if we've already crated a material descriptor for the sub-mesh.
		if (pipeline.m_pMaterialDescriptors.contains(subMesh))
			return pipeline.m_pMaterialDescriptors[subMesh].get();

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

		return pDescriptor.get();
	}

	void DefaultRasterizingLayer::issueDrawCalls()
	{
		OPTICK_EVENT();

		// Return without doing anything is a scene is not attached.
		if (m_pScene == nullptr)
			return;

		// Reset the counters.
		m_DrawCount = 0;
		const auto subMeshCount = m_pScene->getDrawableCount();

		// Return if we have nothing to draw.
		if (subMeshCount == 0)
			return;

		// Reset the synchronization primitive.
		m_Synchronization.reset(subMeshCount);

		// Begin the command recorders and set the viewport and scissor.
		for (const auto& [id, pCommandRecorder] : m_pThreadLocalCommandRecorder)
		{
			OPTICK_EVENT_DYNAMIC("Begin Secondary Recorders");

			// Begin the command recorder.
			pCommandRecorder->begin(m_pCommandRecorder.get());

			// Set the scissor and view port.
			pCommandRecorder->setViewport(0.0f, 0.0f, static_cast<float>(m_Renderer.getCamera()->getWidth()), static_cast<float>(m_Renderer.getCamera()->getHeight()), 0.0f, 1.0f);
			pCommandRecorder->setScissor(0, 0, m_Renderer.getCamera()->getWidth(), m_Renderer.getCamera()->getHeight());
		}

		// Issue the binding calls.
		uint64_t index = 0;
		for (const auto& group : m_pScene->getRegistry().view<Geometry, Material>())
		{
			OPTICK_EVENT_DYNAMIC("Binding Geometry");
			auto& geometry = m_pScene->getRegistry().get<Geometry>(group);
			const auto& material = m_pScene->getRegistry().get<Material>(group);
			const auto& materialSpecification = m_Renderer.getInstance().getMaterialDatabase().getSpecification(material);

			// Create the pipeline if the required material does not exist.
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
				pipeline.m_pSceneDescriptor->attach(0, m_Renderer.getCamera()->getViewports().front().m_pUniformBuffer);
			}

			// Get the pipeline from the storage.
			auto& pipeline = m_pPipelines[material];

			// Bind the sub-meshes.
			for (auto& mesh : geometry.getMeshes())
			{
				OPTICK_EVENT_DYNAMIC("Binding Mesh");

				for (auto& subMesh : mesh.m_SubMeshes)
				{
					OPTICK_EVENT_DYNAMIC("Binding Draw Entry (sub-mesh)");

					DrawEntry entry = {};
					entry.m_SubMesh = subMesh;
					entry.m_VertexSpecification = geometry.getVertexSpecification();
					entry.m_pPipeline = pipeline.m_pPipeline.get();
					entry.m_pVertexBuffer = geometry.getVertexBuffer();
					entry.m_pIndexBuffer = geometry.getIndexBuffer();
					entry.m_pUserDefinedDescriptor = nullptr;
					entry.m_pMaterialDescriptor = getMaterialDescriptor(pipeline, subMesh, materialSpecification);
					entry.m_pSceneDescriptor = pipeline.m_pSceneDescriptor.get();
					entry.m_QueryIndex = index++;

					GetJobSystem().insert([this, entry] { bindingCall(entry); });
				}
			}
		}

		// Wait till all the work is done.
		m_Synchronization.wait();

		// End the command recorders and select the next one.
		for (const auto& [id, pCommandRecorder] : m_pThreadLocalCommandRecorder)
		{
			OPTICK_EVENT_DYNAMIC("End Secondary Recorders");

			// End the command recorder.
			pCommandRecorder->end();

			// Select the next command recorder.
			pCommandRecorder->next();
		}

		// Execute all the secondary command recorders (children).
		m_pCommandRecorder->executeChildren();
	}

	void DefaultRasterizingLayer::bindingCall(const DrawEntry& entry)
	{
		OPTICK_EVENT();

		// Setup the command recorders if we don't have one for this thread.
		const auto id = std::this_thread::get_id();
		if (!m_pThreadLocalCommandRecorder.contains(id))
		{
			auto lock = std::scoped_lock(m_Mutex);

			// Create the thread-specific command recorder.
			const auto& pCommandRecorder = m_pThreadLocalCommandRecorder[id] = m_Renderer.getInstance().getFactory()->createCommandRecorder(
				m_Renderer.getInstance().getBackendDevice(),
				Backend::CommandRecorderUsage::Secondary,
				m_Renderer.getCommandRecorder()->getBufferCount()
			);

			// Begin the command recorder.
			pCommandRecorder->begin(m_pCommandRecorder.get());

			// Set the scissor and view port.
			pCommandRecorder->setViewport(0.0f, 0.0f, static_cast<float>(m_Renderer.getCamera()->getWidth()), static_cast<float>(m_Renderer.getCamera()->getHeight()), 0.0f, 1.0f);
			pCommandRecorder->setScissor(0, 0, m_Renderer.getCamera()->getWidth(), m_Renderer.getCamera()->getHeight());
		}

#ifdef ENABLE_OCCLUSION_CULL
		// Run the occlusion pass on the entry.
		occlusionPass(entry);

#endif // ENABLE_OCCLUSION_CULL

		// Run the geometry pass on the entry.
		geometryPass(entry);

		// Notify the parent that we're done.
		m_Synchronization.arrive();
	}

	void DefaultRasterizingLayer::occlusionPass(const DrawEntry& entry) const
	{
		OPTICK_EVENT();

		const auto& pCommandRecorder = m_pThreadLocalCommandRecorder.at(std::this_thread::get_id());

		pCommandRecorder->bind(m_pOcclusionPipeline.get(), entry.m_VertexSpecification);
		pCommandRecorder->bind(entry.m_pVertexBuffer, entry.m_VertexSpecification.getSize());
		pCommandRecorder->bind(entry.m_pIndexBuffer, static_cast<Backend::IndexBufferStride>(entry.m_SubMesh.m_IndexSize));
		pCommandRecorder->bind(m_pOcclusionPipeline.get(), nullptr, nullptr, m_pOcclusionCameraDescriptor.get());

		pCommandRecorder->beginQuery(m_pOcclusionQuery.get(), static_cast<uint32_t>(entry.m_QueryIndex));
		pCommandRecorder->drawIndexed(entry.m_SubMesh.m_VertexOffset, entry.m_SubMesh.m_IndexOffset, entry.m_SubMesh.m_IndexCount);
		pCommandRecorder->endQuery(m_pOcclusionQuery.get(), static_cast<uint32_t>(entry.m_QueryIndex));
	}

	void DefaultRasterizingLayer::geometryPass(const DrawEntry& entry)
	{
		OPTICK_EVENT();

		const auto& pCommandRecorder = m_pThreadLocalCommandRecorder.at(std::this_thread::get_id());

#ifdef ENABLE_OCCLUSION_CULL
		// Draw only if we're in the camera's view frustum.
		if (m_pOcclusionQuery->getSamples()[entry.m_QueryIndex] > 0)

#endif // ENABLE_OCCLUSION_CULL

		{
			OPTICK_EVENT_DYNAMIC("Issuing Draw Calls");

			pCommandRecorder->bind(entry.m_pPipeline, entry.m_VertexSpecification);
			pCommandRecorder->bind(entry.m_pVertexBuffer, entry.m_VertexSpecification.getSize());
			pCommandRecorder->bind(entry.m_pIndexBuffer, static_cast<Backend::IndexBufferStride>(entry.m_SubMesh.m_IndexSize));
			pCommandRecorder->bind(entry.m_pPipeline, entry.m_pUserDefinedDescriptor, entry.m_pMaterialDescriptor, entry.m_pSceneDescriptor);

			pCommandRecorder->drawIndexed(entry.m_SubMesh.m_VertexOffset, entry.m_SubMesh.m_IndexOffset, entry.m_SubMesh.m_IndexCount);

			m_DrawCount++;
		}
	}
}