// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "HostAccessibleBuffer.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Uniform buffer class.
		 * This class is used to send uniform information to the shaders when rendering.
		 */
		class UniformBuffer : public HostAccessibleBuffer
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param size The size of the buffer in bytes.
			 */
			explicit UniformBuffer(Device* pDevice, uint64_t size) : HostAccessibleBuffer(pDevice, size) {}

			/**
			 * Default virtual destructor.
			 */
			virtual ~UniformBuffer() = default;
		};
	}
}