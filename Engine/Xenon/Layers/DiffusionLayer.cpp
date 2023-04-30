// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#include "DiffusionLayer.hpp"

#include "../Renderer.hpp"
#include "../DefaultCacheHandler.hpp"

#include "../../XenonShaderBank/Diffusion/MipMapGenerator.comp.hpp"

#include <optick.h>

namespace Xenon
{
	namespace Experimental
	{
		DiffusionLayer::DiffusionLayer(Renderer& renderer, uint32_t width, uint32_t height, uint32_t priority)
			: Layer(renderer, priority)
			, m_pMipMapGenerationPipeline(renderer.getInstance().getFactory()->createComputePipeline(renderer.getInstance().getBackendDevice(), std::make_unique<DefaultCacheHandler>(), Generated::CreateShaderMipMapGenerator_comp()))
			, m_ImageLayers(static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1)
		{
			// Setup the descriptors.
			m_pMipMapGenerationDescriptor = m_pMipMapGenerationPipeline->createDescriptor(Backend::DescriptorType::UserDefined);

			// Setup the images.
			Backend::ImageSpecification specification = {};
			specification.m_Width = width;
			specification.m_Height = height;
			specification.m_Format = Backend::DataFormat::R8G8B8A8_UNORMAL;
			specification.m_Usage = Backend::ImageUsage::Graphics;
			m_pScalingImage = renderer.getInstance().getFactory()->createImage(getInstance().getBackendDevice(), specification);
			m_pScalingImageView = getInstance().getFactory()->createImageView(getInstance().getBackendDevice(), m_pScalingImage.get(), {});

			specification.m_Format = Backend::DataFormat::R32_SFLOAT;
			specification.m_Usage = Backend::ImageUsage::Storage | Backend::ImageUsage::Graphics;
			m_pIlluminationImage = getInstance().getFactory()->createImage(getInstance().getBackendDevice(), specification);

			// Bind the images to the required descriptors.

			// Create the default diffusion pass.
			m_pDiffusionPass = createPass<DiffusionPass>(width, height);
		}

		void DiffusionLayer::onUpdate(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex)
		{
			OPTICK_EVENT();

			m_pCommandRecorder->begin();

			runPasses(pPreviousLayer, imageIndex, frameIndex);

			m_pCommandRecorder->end();
		}

		void DiffusionLayer::setSourceImage(Backend::Image* pImage)
		{
			OPTICK_EVENT();

			getInstance().getBackendDevice()->waitIdle();
			m_pDiffusionPass->setSourceImage(pImage);
		}
	}
}