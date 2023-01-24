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
		DiffusionLayer::DiffusionLayer(Renderer& renderer, uint32_t priority)
			: Layer(renderer, priority)
			, m_pMipMapGenerationPipeline(renderer.getInstance().getFactory()->createComputePipeline(renderer.getInstance().getBackendDevice(), std::make_unique<DefaultCacheHandler>(), Generated::CreateShaderMipMapGenerator_comp()))
			, m_pDiffusionPipeline(renderer.getInstance().getFactory()->createComputePipeline(renderer.getInstance().getBackendDevice(), std::make_unique<DefaultCacheHandler>(), Generated::CreateShaderShader_comp()))
		{
			m_pMipMapGenerationDescriptor = m_pMipMapGenerationPipeline->createDescriptor(Backend::DescriptorType::UserDefined);
			m_pDiffusionDescriptor = m_pDiffusionPipeline->createDescriptor(Backend::DescriptorType::UserDefined);
		}

		void DiffusionLayer::onUpdate(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex)
		{
			m_pCommandRecorder->begin();

			if (m_pSourceImage)
			{
				// Generate the mip-maps using the source image.
				m_pCommandRecorder->bind(m_pMipMapGenerationPipeline.get());
				m_pCommandRecorder->bind(m_pMipMapGenerationPipeline.get(), m_pMipMapGenerationDescriptor.get());
				m_pCommandRecorder->compute(m_pSourceImage->getWidth(), m_pSourceImage->getHeight(), m_pSourceImage->getDepth());

				// Use the mip-maps to perform diffusion.
				m_pCommandRecorder->bind(m_pDiffusionPipeline.get());
				m_pCommandRecorder->bind(m_pDiffusionPipeline.get(), m_pDiffusionDescriptor.get());
				m_pCommandRecorder->compute(m_pSourceImage->getWidth(), m_pSourceImage->getHeight(), m_pSourceImage->getDepth());
			}

			m_pCommandRecorder->end();
		}
	}
}