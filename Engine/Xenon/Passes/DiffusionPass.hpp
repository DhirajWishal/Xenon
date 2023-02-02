// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../LayerPass.hpp"

#include "../../XenonBackend/ComputePipeline.hpp"

namespace Xenon
{
	namespace Experimental
	{
		/**
		 * Diffusion pass class.
		 * This class will perform the actual ray diffusion.
		 */
		class DiffusionPass final : public LayerPass
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param layer The layer reference to which the pass is attached to.
			 * @param width The width of the layer's output image.
			 * @param height The height of the layer's output image.
			 */
			explicit DiffusionPass(Layer& layer, uint32_t width, uint32_t height);

			/**
			 * On update pure virtual function.
			 * This class is invoked by the layer when it's onUpdate method is called.
			 *
			 * @param pPreviousLayer The previous layer pointer.
			 * @param imageIndex The swapchain image index.
			 * @param frameIndex The current frame index.
			 * @param pCommandRecorder The command recorder pointer to bind the commands to.
			 */
			void onUpdate(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex, Backend::CommandRecorder* pCommandRecorder) override;

			/**
			 * Set the source image which is used to perform all the diffuse operations.
			 *
			 * @param pImage The source image pointer.
			 */
			void setSourceImage(Backend::Image* pImage);

		private:
			std::unique_ptr<Backend::ComputePipeline> m_pPipeline = nullptr;

			std::unique_ptr<Backend::Descriptor> m_pDescriptor = nullptr;

			std::unique_ptr<Backend::Image> m_pOutputImage = nullptr;
			std::unique_ptr<Backend::ImageView> m_pOutputImageView = nullptr;
			std::unique_ptr<Backend::ImageSampler> m_pImageSampler = nullptr;

			std::unique_ptr<Backend::ImageView> m_pSourceImageView = nullptr;
			Backend::Image* m_pSourceImage = nullptr;
		};
	}
}