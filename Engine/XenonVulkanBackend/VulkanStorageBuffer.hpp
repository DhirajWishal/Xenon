// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/StorageBuffer.hpp"

#include "VulkanBuffer.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan storage buffer class.
		 */
		class VulkanStorageBuffer final : public StorageBuffer, public VulkanBuffer
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param size The size of the buffer in bytes.
			 */
			explicit VulkanStorageBuffer(VulkanDevice* pDevice, uint64_t size);

			/**
			 * Destructor.
			 */
			~VulkanStorageBuffer() override = default;

			/**
			 * Copy data from another buffer to this buffer.
			 *
			 * @param pBuffer The buffer to copy the data from.
			 * @param size The size in bytes to copy.
			 * @param srcOffset The source buffer's offset. Default is 0.
			 * @param dstOffset The destination buffer's (this) offset. Default is 0.
			 */
			void copy(const Buffer* pBuffer, uint64_t size, uint64_t srcOffset = 0, uint64_t dstOffset = 0) override;

			/**
			 * Map the buffer memory for host access.
			 *
			 * @return The buffer memory.
			 */
			[[nodiscard]] std::byte* map() override;

			/**
			 * Unmap the buffer memory from host access.
			 */
			void unmap() override;
		};
	}
}