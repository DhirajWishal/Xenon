// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../Layer.hpp"

#include "../Passes/DiffusionPass.hpp"

namespace Xenon
{
	namespace Experimental
	{
		/**
		 * Diffusion layer class.
		 * This layer can be used to perform bloom and anti-aliasing (from what I can tell).
		 *
		 * This uses a compute shader to build multiple down-samples of an incoming image (also called mip-mapping). These images are then used to perform, almost a copy
		 * over the same source image to calculate light variations. This gives rise to bloom and maybe anti-aliasing.
		 *
		 * The primary goal of this layer is to compute bloom and to use the output as a cheap way of approximating indirect lighting over a scene.
		 *
		 * Names:
		 * 1. Screen Space Lighting (SSL)
		 * 2. Screen Space Global Illumination (SSGI)
		 * 3. Screen Space Light Diffusion (SSLD)
		 * 4. Differed Light Diffusion (DLD)
		 */
		class DiffusionLayer final : public Layer
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param renderer The renderer reference.
			 * @param width The width of the layer's output image.
			 * @param height The height of the layer's output image.
			 * @param priority The priority of the layer.
			 */
			explicit DiffusionLayer(Renderer& renderer, uint32_t width, uint32_t height, uint32_t priority);

			/**
			 * Update the layer.
			 * This is called by the renderer and all the required commands must be updated (if required) in this call.
			 *
			 * @param pPreviousLayer The previous layer pointer. This will be nullptr if this layer is the first.
			 * @param imageIndex The image's index.
			 * @param frameIndex The frame's index.
			 */
			void onUpdate(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex) override;

			/**
			 * Get the color attachment from the layer.
			 *
			 * @return The image pointer.
			 */
			[[nodiscard]] Backend::Image* getColorAttachment() override { return m_pDiffusionPass->getOutputImage(); }

			/**
			 * Set the source image pointer to perform diffusion.
			 *
			 * @param pImage The image pointer.
			 */
			void setSourceImage(Backend::Image* pImage);

		private:
			std::unique_ptr<Backend::ComputePipeline> m_pMipMapGenerationPipeline = nullptr;

			std::unique_ptr<Backend::Descriptor> m_pMipMapGenerationDescriptor = nullptr;

			std::unique_ptr<Backend::Image> m_pScalingImage = nullptr;

			std::unique_ptr<Backend::Image> m_pIlluminationImage = nullptr;

			std::unique_ptr<Backend::ImageView> m_pScalingImageView = nullptr;

			DiffusionPass* m_pDiffusionPass = nullptr;

			uint32_t m_ImageLayers = 0;
		};
	}
}