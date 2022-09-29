// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Buffer.hpp"

#include <cstddef>

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Host accessible buffer class.
		 * This buffer type can be used by the host (the user) to access data directly.
		 */
		class HostAccessibleBuffer : public Buffer
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param size The size of the buffer in bytes.
			 */
			explicit HostAccessibleBuffer(Device* pDevice, uint64_t size) : Buffer(pDevice, size) {}

			/**
			 * Default virtual destructor.
			 */
			virtual ~HostAccessibleBuffer() = default;

			/**
			 * Map the buffer memory for host access.
			 *
			 * @return The buffer memory.
			 */
			[[nodiscard]] virtual std::byte* map() = 0;

			/**
			 * Unmap the buffer memory from host access.
			 */
			virtual void unmap() = 0;

			/**
			 * Utility method to copy data from any type of data.
			 *
			 * @tparam Type The data type.
			 * @param data The data pointer to copy.
			 * @param size The data size to copy. Default is the size of the Type.
			 * @param offset The buffer offset to copy to. Default is 0.
			 */
			template<class Type>
			void copy(const Type* data, uint64_t size = sizeof(Type), uint64_t offset = 0)
			{
				auto destination = map() + offset;
				std::copy_n(reinterpret_cast<const std::byte*>(data), size, destination);
				unmap();
			}
		};
	}
}