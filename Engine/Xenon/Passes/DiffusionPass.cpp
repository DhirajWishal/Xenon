// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DiffusionPass.hpp"

#include "../Layer.hpp"
#include "../Renderer.hpp"
#include "../DefaultCacheHandler.hpp"

#include "../../XenonShaderBank/Diffusion/Shader.comp.hpp"

#include <optick.h>

namespace Xenon
{
	namespace Experimental
	{
		DiffusionPass::DiffusionPass(Layer& layer, uint32_t width, uint32_t height)
			: LayerPass(layer)
			, m_pPipeline(layer.getInstance().getFactory()->createComputePipeline(layer.getInstance().getBackendDevice(), std::make_unique<DefaultCacheHandler>(), Generated::CreateShaderShader_comp()))
			, m_pImageSampler(layer.getInstance().getFactory()->createImageSampler(layer.getInstance().getBackendDevice(), {}))
		{
			// Setup the output image.
			Backend::ImageSpecification specification = {};
			specification.m_Width = width;
			specification.m_Height = height;
			specification.m_Format = Backend::DataFormat::R8G8B8A8_UNORMAL;
			specification.m_Usage = Backend::ImageUsage::Storage | Backend::ImageUsage::Graphics;
			m_pOutputImage = layer.getInstance().getFactory()->createImage(layer.getInstance().getBackendDevice(), specification);
			m_pOutputImageView = layer.getInstance().getFactory()->createImageView(layer.getInstance().getBackendDevice(), m_pOutputImage.get(), {});

			// Setup the descriptor.
			m_pDescriptor = m_pPipeline->createDescriptor(Backend::DescriptorType::UserDefined);

			// Attach everything that we need.
			m_pDescriptor->attach(1, m_pOutputImage.get(), m_pOutputImageView.get(), m_pImageSampler.get(), Backend::ImageUsage::Storage);
		}

		void DiffusionPass::onUpdate(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex, Backend::CommandRecorder* pCommandRecorder)
		{
			OPTICK_EVENT();

			pCommandRecorder->bind(m_pPipeline.get());
			pCommandRecorder->bind(m_pPipeline.get(), m_pDescriptor.get());
			pCommandRecorder->compute(m_pSourceImage->getWidth() / 8, m_pSourceImage->getHeight() / 8, m_pSourceImage->getDepth());
		}

		void DiffusionPass::setSourceImage(Backend::Image* pImage)
		{
			OPTICK_EVENT();

			m_pSourceImage = pImage;
			m_pSourceImageView = getLayer().getInstance().getFactory()->createImageView(getLayer().getInstance().getBackendDevice(), pImage, {});

			m_pDescriptor->attach(0, m_pSourceImage, m_pSourceImageView.get(), m_pImageSampler.get(), Backend::ImageUsage::Graphics);
		}
	}
}