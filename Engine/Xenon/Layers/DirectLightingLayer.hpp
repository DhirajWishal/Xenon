// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../Layer.hpp"

#include "../../XenonBackend/ComputePipeline.hpp"

#include "GBufferLayer.hpp"
#include "LightLUT.hpp"

namespace Xenon
{
	namespace Experimental
	{
		/**
		 * Direct lighting layer class.
		 * This attempts to calculate direct lighting using screen space data.
		 */
		class DirectLightingLayer final : public Layer
		{
			/**
			 * Control structure.
			 */
			struct ControlStructure final
			{
				XENON_HLSL_VEC3_ALIGNMENT uint32_t m_LightCount = 0;
			};

		public:
			/**
			 * Explicit constructor.
			 *
			 * @param renderer The renderer reference.
			 * @param width The width of the layer's output image.
			 * @param height The height of the layer's output image.
			 * @param priority The priority of the layer. Default is 1.
			 */
			explicit DirectLightingLayer(Renderer& renderer, uint32_t width, uint32_t height, uint32_t priority = 1);

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

			/**
			 * Get the GBuffer layer to the lighting layer.
			 *
			 * @param pLayer The layer pointer.
			 */
			void setGBuffer(GBufferLayer* pLayer);

			/**
			 * Set the light look up table.
			 * 
			 * @param pLayer The layer pointer.
			 */
			void setLightLUT(LightLUT* pLayer);

		private:
			/**
			 * Setup buffers for the frame.
			 */
			void setupBuffers();

		private:
			std::unique_ptr<Backend::ComputePipeline> m_pPipeline = nullptr;
			std::unique_ptr<Backend::Descriptor> m_pDescriptor = nullptr;

			std::unique_ptr<Backend::Buffer> m_pControlStructureBuffer = nullptr;
			std::unique_ptr<Backend::Buffer> m_pLightSourcesBuffer = nullptr;

			std::unique_ptr<Backend::Image> m_pOutputImage = nullptr;
			std::unique_ptr<Backend::ImageView> m_pOutputImageView = nullptr;

			std::unique_ptr<Backend::ImageSampler> m_DefaultSampler = nullptr;

			std::array<std::unique_ptr<Backend::ImageView>, 6> m_pColorImageViews = {};
			std::array<std::unique_ptr<Backend::ImageView>, 6> m_pNormalImageViews = {};
			std::array<std::unique_ptr<Backend::ImageView>, 6> m_pPositionImageViews = {};

			ControlStructure m_ControlStructure = {};
		};
	}
}