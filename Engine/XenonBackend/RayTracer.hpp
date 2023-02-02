// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "RenderTarget.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Ray tracer class.
		 * This class is used to store the ray-tracing output which then can be used for any other purpose.
		 */
		class RayTracer : public RenderTarget
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param width The width of the render target.
			 * @param height The height of the render target.
			 */
			explicit RayTracer(const Device* pDevice, uint32_t width, uint32_t height) : RenderTarget(pDevice, width, height, AttachmentType::Color) {}
		};
	}
}