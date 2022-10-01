// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/UniformBuffer.hpp"

#include "VulkanBuffer.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan uniform buffer class.
		 */
		class VulkanUniformBuffer final : public UniformBuffer, public VulkanBuffer
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param size The size of the buffer in bytes.
			 */
			explicit VulkanUniformBuffer(VulkanDevice* pDevice, uint64_t size);

			/**
			 * Destructor.
			 */
			~VulkanUniformBuffer() override = default;

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
			 * Write data to the buffer.
			 *
			 * @param pData The data pointer to copy the data from.
			 * @param size The size of the data to copy in bytes.
			 * @param offset The buffer's offset to copy to. Default is 0.
			 */
			void write(const std::byte* pData, uint64_t size, uint64_t offset = 0) override;

			/**
			 * Begin reading data from the GPU.
			 *
			 * @return The const data pointer.
			 */
			[[nodiscard]] const std::byte* beginRead() override;

			/**
			 * End the buffer reading.
			 */
			void endRead() override;
		};
	}
}