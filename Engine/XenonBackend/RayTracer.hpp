// Copyright 2022 Dhiraj Wishal
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
			 * @param pCamera The camera pointer.
			 */
			explicit RayTracer(const Device* pDevice, Camera* pCamera) : RenderTarget(pDevice, pCamera, AttachmentType::Color) {}
		};
	}
}