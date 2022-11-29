// Copyright 2022 Dhiraj Wishal
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
		 * @param pCamera The camera which is used to render the scene.
		 * @param attachmentTypes The attachment types the render target should support.
		 * @param enableTripleBuffering Whether to enable triple-buffering. Default is false.
		 * @param multiSampleCount Multi-sampling count to use. Default is x1.
		 */
		explicit RasterizingLayer(
			Renderer& renderer,
			Backend::Camera* pCamera,
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