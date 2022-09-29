// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanStagingBuffer.hpp"
#include "VulkanCommandBuffers.hpp"
#include "VulkanMacros.hpp"

namespace Xenon
{
	namespace Backend
	{
		VulkanStagingBuffer::VulkanStagingBuffer(VulkanDevice* pDevice, uint64_t size)
			: StagingBuffer(pDevice, size)
			, VulkanBuffer(pDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST)
		{
		}

		VulkanStagingBuffer::~VulkanStagingBuffer()
		{
		}

		void VulkanStagingBuffer::copy(const Buffer* pBuffer, uint64_t size, uint64_t srcOffset /*= 0*/, uint64_t dstOffset /*= 0*/)
		{
			auto commandBuffers = VulkanCommandBuffers(m_pDevice);
			commandBuffers.begin();
			commandBuffers.copyBuffers(pBuffer, srcOffset, this, dstOffset, size);
			commandBuffers.end();
			commandBuffers.submitTransfer();
		}

		std::byte* VulkanStagingBuffer::map()
		{
			void* pDataStore = nullptr;

			XENON_VK_ASSERT(vmaMapMemory(m_pDevice->getAllocator(), m_Allocation, &pDataStore), "Failed to map the staging buffer memory!");
			return reinterpret_cast<std::byte*>(pDataStore);
		}

		void VulkanStagingBuffer::unmap()
		{
			vmaUnmapMemory(m_pDevice->getAllocator(), m_Allocation);
		}
	}
}