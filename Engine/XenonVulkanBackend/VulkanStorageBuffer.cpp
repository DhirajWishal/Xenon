// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanStorageBuffer.hpp"
#include "VulkanMacros.hpp"

namespace Xenon
{
	namespace Backend
	{
		VulkanStorageBuffer::VulkanStorageBuffer(VulkanDevice* pDevice, uint64_t size)
			: StorageBuffer(pDevice, size)
			, VulkanBuffer(pDevice, size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST)
		{
		}

		void VulkanStorageBuffer::copy(const Buffer* pBuffer, uint64_t size, uint64_t srcOffset /*= 0*/, uint64_t dstOffset /*= 0*/)
		{
			copyFrom(From(pBuffer), size, srcOffset, dstOffset);
		}

		void VulkanStorageBuffer::write(const std::byte* pData, uint64_t size, uint64_t offset /*= 0*/)
		{
			std::copy_n(pData, size, map());
			unmap();
		}

		const std::byte* VulkanStorageBuffer::beginRead()
		{
			return map();
		}

		void VulkanStorageBuffer::endRead()
		{
			unmap();
		}
	}
}