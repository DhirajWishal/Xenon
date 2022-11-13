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
		 * @param instance The instance reference.
		 * @param pCamera The camera which is used to render the scene.
		 * @param attachmentTypes The attachment types the render target should support.
		 * @param enableTripleBuffering Whether to enable triple-buffering. Default is false.
		 * @param multiSampleCount Multi-sampling count to use. Default is x1.
		 */
		explicit RasterizingLayer(
			Instance& instance,
			uint8_t priority,
			Backend::Camera* pCamera,
			Backend::AttachmentType attachmentTypes,
			bool enableTripleBuffering = false,
			Backend::MultiSamplingCount multiSampleCount = Backend::MultiSamplingCount::x1);

	protected:
		std::unique_ptr<Backend::Rasterizer> m_pRasterizer = nullptr;
	};
}