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
			void* pDataStore = nullptr;
			XENON_VK_ASSERT(vmaMapMemory(m_pDevice->getAllocator(), m_Allocation, &pDataStore), "Failed to map the staging buffer memory!");

			std::copy_n(pData, size, reinterpret_cast<std::byte*>(pDataStore));
			endRead();
		}

		const std::byte* VulkanStorageBuffer::beginRead()
		{
			void* pDataStore = nullptr;

			XENON_VK_ASSERT(vmaMapMemory(m_pDevice->getAllocator(), m_Allocation, &pDataStore), "Failed to map the staging buffer memory!");
			return reinterpret_cast<std::byte*>(pDataStore);
		}

		void VulkanStorageBuffer::endRead()
		{
			vmaUnmapMemory(m_pDevice->getAllocator(), m_Allocation);
		}
	}
}