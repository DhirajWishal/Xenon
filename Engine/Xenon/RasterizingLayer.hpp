// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Layer.hpp"

#include "../XenonBackend/Rasterizer.hpp"

namespace Xenon
{
	/**
	 * Rasterizing layer class.
	 * This class can be used to perform rasterization and the result will be copied to the next layer's color buffer.
	 */
	class RasterizingLayer : public Layer
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param renderer The renderer reference.
		 * @param priority The priority of the layer.
		 * @param width The width of the render target.
		 * @param height The height of the render target.
		 * @param attachmentTypes The attachment types the render target should support.
		 * @param enableTripleBuffering Whether to enable triple-buffering. Default is false.
		 * @param multiSampleCount Multi-sampling count to use. Default is x1.
		 */
		explicit RasterizingLayer(
			Renderer& renderer,
			uint32_t priority,
			uint32_t width, 
			uint32_t height,
			Backend::AttachmentType attachmentTypes,
			bool enableTripleBuffering = false,
			Backend::MultiSamplingCount multiSampleCount = Backend::MultiSamplingCount::x1);

		/**
		 * Get the color attachment from the layer.
		 *
		 * @return The image pointer.
		 */
		[[nodiscard]] Backend::Image* getColorAttachment() override;

		/**
		 * Get the internally stored rasterizer.
		 *
		 * @return The rasterizer pointer.
		 */
		[[nodsicard]] Backend::Rasterizer* getRasterizer() { return m_pRasterizer.get(); }

		/**
		 * Get the internally stored rasterizer.
		 *
		 * @return The rasterizer pointer.
		 */
		[[nodsicard]] const Backend::Rasterizer* getRasterizer() const { return m_pRasterizer.get(); }

	protected:
		std::unique_ptr<Backend::Rasterizer> m_pRasterizer = nullptr;
	};
}