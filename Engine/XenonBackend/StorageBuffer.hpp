// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "HostAccessibleBuffer.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Storage buffer class.
		 * This class is used to send storage information to the shaders when rendering or when performing compute operations.
		 */
		class StorageBuffer : public HostAccessibleBuffer
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param size The size of the buffer in bytes.
			 */
			explicit StorageBuffer(Device* pDevice, uint64_t size) : HostAccessibleBuffer(pDevice, size, BufferType::Storage) {}

			/**
			 * Default virtual destructor.
			 */
			virtual ~StorageBuffer() = default;
		};
	}
}