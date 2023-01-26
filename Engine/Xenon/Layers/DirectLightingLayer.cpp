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
		}

		void DirectLightingLayer::onUpdate(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex)
		{
			OPTICK_EVENT();

			m_pCommandRecorder->begin();

			// Issue the draw calls.
			m_pCommandRecorder->bind(m_pPipeline.get());
			m_pCommandRecorder->bind(m_pPipeline.get(), m_pDescriptor.get());
			m_pCommandRecorder->compute(m_pOutputImage->getWidth() / 8, m_pOutputImage->getHeight() / 8, m_pOutputImage->getDepth());

			m_pCommandRecorder->end();
		}

		void DirectLightingLayer::setGBuffer(GBufferLayer* pLayer)
		{
			const auto imageFace = EnumToInt(pLayer->getFace());
			m_pColorImageViews[imageFace] = m_Renderer.getInstance().getFactory()->createImageView(m_Renderer.getInstance().getBackendDevice(), pLayer->getColorAttachment(), {});
			m_pNormalImageViews[imageFace] = m_Renderer.getInstance().getFactory()->createImageView(m_Renderer.getInstance().getBackendDevice(), pLayer->getNormalAttachment(), {});

			m_pDescriptor->attach(1 + imageFace, pLayer->getColorAttachment(), m_pColorImageViews[imageFace].get(), m_DefaultSampler.get(), Backend::ImageUsage::Graphics);
			m_pDescriptor->attach(7 + imageFace, pLayer->getNormalAttachment(), m_pNormalImageViews[imageFace].get(), m_DefaultSampler.get(), Backend::ImageUsage::Graphics);
		}
	}
}