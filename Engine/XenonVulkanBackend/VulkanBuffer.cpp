// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBuffer.hpp"
#include "VulkanMacros.hpp"

#include "VulkanCommandRecorder.hpp"

namespace Xenon
{
	namespace Backend
	{
		VulkanBuffer::VulkanBuffer(VulkanDevice* pDevice, uint64_t size, BufferType type)
			: VulkanDeviceBoundObject(pDevice)
			, Buffer(pDevice, size, type)
		{
			// Setup buffer and memory flags.
			VkBufferUsageFlags usageFlags = 0;
			VmaAllocationCreateFlags vmaFlags = 0;
			VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO;

			switch (type)
			{
			case Xenon::Backend::BufferType::Index:
				m_pTemporaryBuffer = std::make_unique<VulkanBuffer>(m_pDevice, getSize(), Xenon::Backend::BufferType::Staging);
				usageFlags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
				break;

			case Xenon::Backend::BufferType::Vertex:
				m_pTemporaryBuffer = std::make_unique<VulkanBuffer>(m_pDevice, getSize(), Xenon::Backend::BufferType::Staging);
				usageFlags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
				break;

			case Xenon::Backend::BufferType::Staging:
				usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
				vmaFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
				break;

			case Xenon::Backend::BufferType::Storage:
				usageFlags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
				vmaFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
				break;

			case Xenon::Backend::BufferType::Uniform:
				usageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
				vmaFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
				break;

			default:
				m_Type = Xenon::Backend::BufferType::Staging;
				usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
				XENON_LOG_ERROR("Invalid or unsupported buffer type! Defaulting to staging.");
				break;
			}

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
			m_pDevice->getInstance()->getDeletionQueue().insert([pDevice = m_pDevice, buffer = m_Buffer, allocation = m_Allocation]
				{
					vmaDestroyBuffer(pDevice->getAllocator(), buffer, allocation);
				}
			);
		}

		void VulkanBuffer::copy(Buffer* pBuffer, uint64_t size, uint64_t srcOffset /*= 0*/, uint64_t dstOffset /*= 0*/)
		{
			auto commandBuffers = VulkanCommandRecorder(m_pDevice, CommandRecorderUsage::Transfer);
			commandBuffers.begin();
			commandBuffers.copyBuffer(pBuffer, srcOffset, this, dstOffset, size);
			commandBuffers.end();
			commandBuffers.submit();
			commandBuffers.wait();
		}

		void VulkanBuffer::write(const std::byte* pData, uint64_t size, uint64_t offset /*= 0*/)
		{
			// If the buffer is either index of vertex, copy to a staging buffer before writing.
			if (m_Type == BufferType::Index || m_Type == BufferType::Vertex)
			{
				auto buffer = VulkanBuffer(m_pDevice, size, BufferType::Staging);
				buffer.write(pData, size, 0);

				copy(&buffer, size, offset);
			}
			else
			{
				std::copy_n(pData, size, map());
				unmap();
			}
		}

		const std::byte* VulkanBuffer::beginRead()
		{
			// If the buffer is either index of vertex, copy to a staging buffer before reading.
			if (m_Type == BufferType::Index || m_Type == BufferType::Vertex)
			{
				// Copy the data and map the temporary buffer.
				m_pTemporaryBuffer->copy(this, getSize());
				return m_pTemporaryBuffer->map();
			}

			return map();
		}

		void VulkanBuffer::endRead()
		{
			if (m_Type == BufferType::Index || m_Type == BufferType::Vertex)
				m_pTemporaryBuffer->unmap();

			else
				unmap();
		}

		std::byte* VulkanBuffer::map()
		{
			void* pDataStore = nullptr;

			XENON_VK_ASSERT(vmaMapMemory(m_pDevice->getAllocator(), m_Allocation, &pDataStore), "Failed to map the staging buffer memory!");
			return reinterpret_cast<std::byte*>(pDataStore);
		}

		void VulkanBuffer::unmap()
		{
			vmaUnmapMemory(m_pDevice->getAllocator(), m_Allocation);
		}
	}
}