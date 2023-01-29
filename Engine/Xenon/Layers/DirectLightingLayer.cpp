// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DirectLightingLayer.hpp"

#include "../Renderer.hpp"
#include "../DefaultCacheHandler.hpp"

#include "../../XenonShaderBank/DirectLighting/DirectLighting.comp.hpp"

#include <optick.h>

namespace Xenon
{
	namespace Experimental
	{
		DirectLightingLayer::DirectLightingLayer(Renderer& renderer, uint32_t width, uint32_t height, uint32_t priority)
			: Layer(renderer, priority)
			, m_pPipeline(renderer.getInstance().getFactory()->createComputePipeline(renderer.getInstance().getBackendDevice(), std::make_unique<DefaultCacheHandler>(), Generated::CreateShaderDirectLighting_comp()))
			, m_pDescriptor(m_pPipeline->createDescriptor(Backend::DescriptorType::UserDefined))
			, m_pControlStructureBuffer(renderer.getInstance().getFactory()->createBuffer(renderer.getInstance().getBackendDevice(), sizeof(ControlStructure), Backend::BufferType::Uniform))
			, m_pLightSourcesBuffer(renderer.getInstance().getFactory()->createBuffer(renderer.getInstance().getBackendDevice(), sizeof(Components::LightSource)* XENON_MAX_LIGHT_SOURCE_COUNT, Backend::BufferType::Uniform))
			, m_DefaultSampler(renderer.getInstance().getFactory()->createImageSampler(renderer.getInstance().getBackendDevice(), {}))
		{
			// Create the output image.
			Backend::ImageSpecification specification = {};
			specification.m_Width = width;
			specification.m_Height = height;
			specification.m_Format = Backend::DataFormat::R8G8B8A8_UNORMAL;
			specification.m_Usage = Backend::ImageUsage::Storage | Backend::ImageUsage::Graphics;
			m_pOutputImage = renderer.getInstance().getFactory()->createImage(renderer.getInstance().getBackendDevice(), specification);
			m_pOutputImageView = renderer.getInstance().getFactory()->createImageView(renderer.getInstance().getBackendDevice(), m_pOutputImage.get(), {});

			// Attach the output image.
			m_pDescriptor->attach(0, m_pOutputImage.get(), m_pOutputImageView.get(), m_DefaultSampler.get(), Backend::ImageUsage::Storage);
			m_pDescriptor->attach(1, m_pControlStructureBuffer.get());
			m_pDescriptor->attach(2, m_pLightSourcesBuffer.get());
		}

		void DirectLightingLayer::onUpdate(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex)
		{
			OPTICK_EVENT();

			// Copy the data to the required buffers.
			setupBuffers();

			// Begin the command recorder.
			m_pCommandRecorder->begin();

			// Issue the draw calls.
			m_pCommandRecorder->bind(m_pPipeline.get());
			m_pCommandRecorder->bind(m_pPipeline.get(), m_pDescriptor.get());
			m_pCommandRecorder->compute(m_pOutputImage->getWidth() / 8, m_pOutputImage->getHeight() / 8, m_pOutputImage->getDepth());

			// End the command recorder.
			m_pCommandRecorder->end();
		}

		void DirectLightingLayer::setGBuffer(GBufferLayer* pLayer)
		{
			const auto imageFace = EnumToInt(pLayer->getFace());
			m_pColorImageViews[imageFace] = m_Renderer.getInstance().getFactory()->createImageView(m_Renderer.getInstance().getBackendDevice(), pLayer->getColorAttachment(), {});
			m_pNormalImageViews[imageFace] = m_Renderer.getInstance().getFactory()->createImageView(m_Renderer.getInstance().getBackendDevice(), pLayer->getNormalAttachment(), {});
			m_pPositionImageViews[imageFace] = m_Renderer.getInstance().getFactory()->createImageView(m_Renderer.getInstance().getBackendDevice(), pLayer->getPositionAttachment(), {});

			uint32_t offset = 3;
			m_pDescriptor->attach(offset + imageFace, pLayer->getColorAttachment(), m_pColorImageViews[imageFace].get(), m_DefaultSampler.get(), Backend::ImageUsage::Graphics);

			offset += static_cast<uint32_t>(m_pColorImageViews.size());
			m_pDescriptor->attach(offset + imageFace, pLayer->getNormalAttachment(), m_pNormalImageViews[imageFace].get(), m_DefaultSampler.get(), Backend::ImageUsage::Graphics);

			offset += static_cast<uint32_t>(m_pNormalImageViews.size());
			m_pDescriptor->attach(offset + imageFace, pLayer->getPositionAttachment(), m_pPositionImageViews[imageFace].get(), m_DefaultSampler.get(), Backend::ImageUsage::Graphics);
		}

		void DirectLightingLayer::setLightLUT(LightLUT* pLayer)
		{
			pLayer->setAttachment(this);
			m_pDescriptor->attach(21, pLayer->getControlBlock());
			m_pDescriptor->attach(22, pLayer->getLookUpTable());
		}

		void DirectLightingLayer::setupBuffers()
		{
			OPTICK_EVENT();

			std::vector<Components::LightSource> lightSources;
			for (const auto group : m_pScene->getRegistry().view<Components::LightSource>())
				lightSources.emplace_back(m_pScene->getRegistry().get<Components::LightSource>(group));

			const auto copySize = lightSources.size() * sizeof(Components::LightSource);
			m_pLightSourcesBuffer->write(ToBytes(lightSources.data()), copySize);

			m_ControlStructure.m_LightCount = static_cast<uint32_t>(lightSources.size());
			m_pControlStructureBuffer->writeObject(m_ControlStructure);
		}
	}
}