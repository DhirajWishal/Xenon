// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBuffer.hpp"
#include "VulkanMacros.hpp"

#include "VulkanCommandPool.hpp"

#include "VulkanIndexBuffer.hpp"
#include "VulkanStagingBuffer.hpp"
#include "VulkanStorageBuffer.hpp"
#include "VulkanUniformBuffer.hpp"
#include "VulkanVertexBuffer.hpp"

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

		VulkanBuffer* VulkanBuffer::From(Buffer* pBuffer)
		{
			switch (pBuffer->getType())
			{
			case Xenon::Backend::BufferType::Index:
				return pBuffer->as<VulkanIndexBuffer>();

			case Xenon::Backend::BufferType::Vertex:
				return pBuffer->as<VulkanVertexBuffer>();

			case Xenon::Backend::BufferType::Staging:
				return pBuffer->as<VulkanStagingBuffer>();

			case Xenon::Backend::BufferType::Storage:
				return pBuffer->as<VulkanStorageBuffer>();

			case Xenon::Backend::BufferType::Uniform:
				return pBuffer->as<VulkanUniformBuffer>();

			default:
				XENON_LOG_ERROR("Invalid buffer type!");
				return nullptr;
			}
		}

		const VulkanBuffer* VulkanBuffer::From(const Buffer* pBuffer)
		{
			switch (pBuffer->getType())
			{
			case Xenon::Backend::BufferType::Index:
				return pBuffer->as<VulkanIndexBuffer>();

			case Xenon::Backend::BufferType::Vertex:
				return pBuffer->as<VulkanVertexBuffer>();

			case Xenon::Backend::BufferType::Staging:
				return pBuffer->as<VulkanStagingBuffer>();

			case Xenon::Backend::BufferType::Storage:
				return pBuffer->as<VulkanStorageBuffer>();

			case Xenon::Backend::BufferType::Uniform:
				return pBuffer->as<VulkanUniformBuffer>();

			default:
				XENON_LOG_ERROR("Invalid buffer type!");
				return nullptr;
			}
		}

		void VulkanBuffer::copyFrom(const VulkanBuffer* pBuffer, uint64_t size, uint64_t srcOffset, uint64_t dstOffset)
		{
			auto commandBuffers = VulkanCommandPool(m_pDevice);
			commandBuffers.begin();
			commandBuffers.copyBuffers(pBuffer, srcOffset, this, dstOffset, size);
			commandBuffers.end();
			commandBuffers.submitTransfer();
		}
	}
}