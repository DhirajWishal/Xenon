// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../Layer.hpp"

#include "../../XenonBackend/ComputePipeline.hpp"

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
		 */
		class DiffusionLayer final : public Layer
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param renderer The renderer reference.
			 * @param priority The priority of the layer.
			 */
			explicit DiffusionLayer(Renderer& renderer, uint32_t priority);

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
			[[nodiscard]] Backend::Image* getColorAttachment() override { return m_pOutputImage.get(); }

		private:
			std::unique_ptr<Backend::ComputePipeline> m_pMipMapGenerationPipeline = nullptr;
			std::unique_ptr<Backend::ComputePipeline> m_pDiffusionPipeline = nullptr;

			std::unique_ptr<Backend::Descriptor> m_pMipMapGenerationDescriptor = nullptr;
			std::unique_ptr<Backend::Descriptor> m_pDiffusionDescriptor = nullptr;

			std::unique_ptr<Backend::Image> m_pScalingImage = nullptr;
			std::unique_ptr<Backend::Image> m_pOutputImage = nullptr;

			Backend::Image* m_pSourceImage = nullptr;
		};
	}
}