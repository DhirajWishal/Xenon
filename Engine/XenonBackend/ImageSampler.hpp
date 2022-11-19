// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Image sampler class.
		 * This class specifies how to sample an image from the shader.
		 */
		class ImageSampler : public BackendObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 */
			explicit ImageSampler([[maybe_unused]] Device* pDevice) {}
		};
	}
}