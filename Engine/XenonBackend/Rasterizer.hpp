// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "RenderTarget.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Multi sampling count enum.
		 */
		enum class MultiSamplingCount : uint8_t
		{
			x1,
			x2,
			x4,
			x8,
			x16,
			x32,
			x64
		};

		/**
		 * Rasterizer class.
		 * This class can be used to perform rasterization on a scene.
		 */
		class Rasterizer : public RenderTarget
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param pCamera The camera which is used to render the scene.
			 * @param attachmentTypes The attachment types the render target should support.
			 * @param enableTripleBuffering Whether to enable triple-buffering. Default is false.
			 * @param multiSampleCount Multi-sampling count to use. Default is x1.
			 */
			explicit Rasterizer(Device* pDevice, Camera* pCamera, AttachmentType attachmentTypes, bool enableTripleBuffering = false, MultiSamplingCount multiSampleCount = MultiSamplingCount::x1)
				: RenderTarget(pDevice, pCamera, attachmentTypes), m_bEnableTripleBuffering(enableTripleBuffering), m_MultiSamplingCount(multiSampleCount) {}

			/**
			 * Default virtual destructor.
			 */
			virtual ~Rasterizer() = default;

		public:
			/**
			 * Check if triple buffering is enabled.
			 *
			 * @return True if triple buffering is enabled.
			 * @return False if triple buffering is not enabled.
			 */
			[[nodiscard]] bool isTripleBufferingEnabled() const { return m_bEnableTripleBuffering; }

			/**
			 * Get the enabled multi-sampling count.
			 *
			 * @return The multi-sampling count.
			 */
			[[nodiscard]] MultiSamplingCount getMultiSamplingCount() const { return m_MultiSamplingCount; }

		private:
			bool m_bEnableTripleBuffering = false;
			MultiSamplingCount m_MultiSamplingCount = MultiSamplingCount::x1;
		};
	}
}