// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBuffer.hpp"
#include "VulkanMacros.hpp"

namespace Xenon
{
	namespace Backend
	{
		VulkanBuffer::VulkanBuffer(VulkanDevice* pDevice, uint64_t size, VkBufferUsageFlags usageFlags, VmaMemoryUsage memoryUsage)
			: VulkanDeviceBoundObject(pDevice)
		{
			VmaAllocationCreateFlags vmaFlags = 0;

			// Set the sequential write bit to true if we are not using vertex of index buffer bits.
			if (!(usageFlags & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT || usageFlags & VK_BUFFER_USAGE_INDEX_BUFFER_BIT))
				vmaFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

			// Create the buffer.
			VkBufferCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.size = size;
			createInfo.usage = usageFlags;
			createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;

			VmaAllocationCreateInfo allocationCreateInfo = {};
			allocationCreateInfo.flags = vmaFlags;
			allocationCreateInfo.usage = memoryUsage;

			XENON_VK_ASSERT(vmaCreateBuffer(m_pDevice->getAllocator(), &createInfo, &allocationCreateInfo, &m_Buffer, &m_Allocation, nullptr), "Failed to create the buffer!");

			// Set the descriptor buffer info.
			m_BufferInfo.buffer = m_Buffer;
			m_BufferInfo.offset = 0;
			m_BufferInfo.range = size;
		}

		VulkanBuffer::~VulkanBuffer()
		{
			vmaDestroyBuffer(m_pDevice->getAllocator(), m_Buffer, m_Allocation);;
		}
	}
}