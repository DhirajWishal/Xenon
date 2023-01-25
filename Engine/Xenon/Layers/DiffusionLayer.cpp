// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DiffusionLayer.hpp"

#include "../Renderer.hpp"
#include "../DefaultCacheHandler.hpp"

#include "../../XenonShaderBank/Diffusion/Shader.comp.hpp"
#include "../../XenonShaderBank/Diffusion/MipMapGenerator.comp.hpp"

namespace Xenon
{
	namespace Experimental
	{
		DiffusionLayer::DiffusionLayer(Renderer& renderer, uint32_t width, uint32_t height, uint32_t priority)
			: Layer(renderer, priority)
			, m_pMipMapGenerationPipeline(renderer.getInstance().getFactory()->createComputePipeline(renderer.getInstance().getBackendDevice(), std::make_unique<DefaultCacheHandler>(), Generated::CreateShaderMipMapGenerator_comp()))
			, m_pDiffusionPipeline(renderer.getInstance().getFactory()->createComputePipeline(renderer.getInstance().getBackendDevice(), std::make_unique<DefaultCacheHandler>(), Generated::CreateShaderShader_comp()))
			, m_pControlBlockBuffer(renderer.getInstance().getFactory()->createBuffer(renderer.getInstance().getBackendDevice(), sizeof(ControlBlock), Backend::BufferType::Uniform))
			, m_ImageLayers(static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1)
		{
			// Setup the descriptors.
			m_pMipMapGenerationDescriptor = m_pMipMapGenerationPipeline->createDescriptor(Backend::DescriptorType::UserDefined);
			m_pDiffusionDescriptor = m_pDiffusionPipeline->createDescriptor(Backend::DescriptorType::UserDefined);

			// Setup the images.
			Backend::ImageSpecification specification = {};
			specification.m_Width = width;
			specification.m_Height = height;
			specification.m_Format = Backend::DataFormat::R8G8B8A8_UNORMAL;
			specification.m_Usage = Backend::ImageUsage::Storage | Backend::ImageUsage::Graphics;
			m_pOutputImage = renderer.getInstance().getFactory()->createImage(renderer.getInstance().getBackendDevice(), specification);
			m_pOutputImageView = m_Renderer.getInstance().getFactory()->createImageView(m_Renderer.getInstance().getBackendDevice(), m_pOutputImage.get(), {});

			specification.m_Usage = Backend::ImageUsage::Graphics;
			m_pScalingImage = renderer.getInstance().getFactory()->createImage(renderer.getInstance().getBackendDevice(), specification);
			m_pScalingImageView = m_Renderer.getInstance().getFactory()->createImageView(m_Renderer.getInstance().getBackendDevice(), m_pScalingImage.get(), {});

			m_pImageSampler = m_Renderer.getInstance().getFactory()->createImageSampler(m_Renderer.getInstance().getBackendDevice(), {});

			// Bind the images to the required descriptors.
			m_pDiffusionDescriptor->attach(1, m_pScalingImage.get(), m_pScalingImageView.get(), m_pImageSampler.get(), Backend::ImageUsage::Graphics);
			m_pDiffusionDescriptor->attach(2, m_pOutputImage.get(), m_pOutputImageView.get(), m_pImageSampler.get(), Backend::ImageUsage::Storage);
			m_pDiffusionDescriptor->attach(3, m_pControlBlockBuffer.get());

			m_pMipMapGenerationDescriptor->attach(1, m_pScalingImage.get(), m_pScalingImageView.get(), m_pImageSampler.get(), Backend::ImageUsage::Storage);
		}

		void DiffusionLayer::onUpdate(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex)
		{
			// Copy the control block data.
			m_pControlBlockBuffer->write(ToBytes(&m_ControlBlock), sizeof(ControlBlock));

			m_pCommandRecorder->begin();

			if (m_pSourceImage)
			{
				// Copy the source image to the scaling image.
				m_pScalingImage->copyFrom(m_pSourceImage, m_pCommandRecorder.get());
				
				// Now we can generate mip-maps.
				m_pScalingImage->generateMipMaps(m_pCommandRecorder.get());

				// // Generate the mip-maps using the source image.
				// m_pCommandRecorder->bind(m_pMipMapGenerationPipeline.get());
				// m_pCommandRecorder->bind(m_pMipMapGenerationPipeline.get(), m_pMipMapGenerationDescriptor.get());
				// m_pCommandRecorder->compute(m_pSourceImage->getWidth() / 8, m_pSourceImage->getHeight() / 8, m_pSourceImage->getDepth());

				// Use the mip-maps to perform diffusion.
				m_pCommandRecorder->bind(m_pDiffusionPipeline.get());
				m_pCommandRecorder->bind(m_pDiffusionPipeline.get(), m_pDiffusionDescriptor.get());
				m_pCommandRecorder->compute(m_pSourceImage->getWidth() / 8, m_pSourceImage->getHeight() / 8, m_pSourceImage->getDepth());
			}

			m_pCommandRecorder->end();
		}

		void DiffusionLayer::setSourceImage(Backend::Image* pImage)
		{
			m_Renderer.getInstance().getBackendDevice()->waitIdle();
			m_pSourceImage = pImage;
			m_pSourceImageView = m_Renderer.getInstance().getFactory()->createImageView(m_Renderer.getInstance().getBackendDevice(), pImage, {});

			m_pDiffusionDescriptor->attach(0, m_pSourceImage, m_pSourceImageView.get(), m_pImageSampler.get(), Backend::ImageUsage::Graphics);
			m_pMipMapGenerationDescriptor->attach(0, m_pSourceImage, m_pSourceImageView.get(), m_pImageSampler.get(), Backend::ImageUsage::Graphics);
		}
	}
}