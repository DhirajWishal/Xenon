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
		 * Light look up table class.
		 * This class cache's light's occlusion data.
		 *
		 * The process goes as follows.
		 * 1. Compute the world position of a given vertex where the light is the origin.
		 * 2. Take the angle between the light position (origin) and the vertex position and calculate the angle.
		 * 3. Find the correct pixel to which the angle corresponds and mark it as occluded (non-zero value).
		 *
		 * This also refresh the image by setting a single value (0) before each run. And each class is intended to be used by one light.
		 */
		class LightLUT final : public Layer
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
			explicit LightLUT(Renderer& renderer, uint32_t width, uint32_t height, uint32_t priority);

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
			[[nodiscard]] Backend::Image* getColorAttachment() override { return nullptr; }
		};
	}
}