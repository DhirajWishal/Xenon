// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../RasterizingLayer.hpp"

namespace Xenon
{
	namespace Experimental
	{
		/**
		 * GBuffer face enum.
		 */
		enum class GBufferFace : uint8_t
		{
			PositiveX,	// +X
			NegativeX,	// -X

			PositiveY,	// +Y
			NegativeY,	// -Y

			PositiveZ,	// +Z
			NegativeZ,	// -Z

			Front = PositiveZ
		};

		/**
		 * GBuffer layer class.
		 * This class can be used to store geometry information, specifically the following
		 * 1. Color image.
		 * 2. Depth image.
		 * 3. Normal image.
		 * 4. Roughness image.
		 *
		 * This additionally takes a face since the geometry information can be computed to 6 faces (360 degrees).
		 */
		class GBufferLayer final : public RasterizingLayer
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param renderer The renderer reference.
			 * @param pCamera The camera pointer used by the renderer.
			 * @param face The camera's current face. Default is front (no change to the camera).
			 * @param priority The priority of the layer. Default is 0.
			 */
			explicit GBufferLayer(Renderer& renderer, Backend::Camera* pCamera, GBufferFace face = GBufferFace::Front, uint32_t priority = 0);

			/**
			 * Update the layer.
			 * This is called by the renderer and all the required commands must be updated (if required) in this call.
			 *
			 * @param pPreviousLayer The previous layer pointer. This will be nullptr if this layer is the first.
			 * @param imageIndex The image's index.
			 * @param frameIndex The frame's index.
			 */
			void onUpdate(Layer* pPreviousLayer, uint32_t imageIndex, uint32_t frameIndex) override;

		private:
			glm::mat4 m_RotationMatrix = glm::mat4(1.0f);


			GBufferFace m_Face = GBufferFace::Front;
		};
	}
}