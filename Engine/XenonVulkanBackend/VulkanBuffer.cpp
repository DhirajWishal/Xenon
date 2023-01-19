// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBuffer.hpp"
#include "VulkanMacros.hpp"
#include "VulkanCommandRecorder.hpp"

#include <optick.h>

namespace Xenon
{
	namespace Backend
	{
		VulkanBuffer::VulkanBuffer(VulkanDevice* pDevice, uint64_t size, BufferType type)
			: Buffer(pDevice, size, type)
			, VulkanDeviceBoundObject(pDevice)
		{
			// Setup buffer and memory flags.
			VkBufferUsageFlags usageFlags = 0;
			VmaAllocationCreateFlags vmaFlags = 0;
			VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO;

			switch (type)
			{
			case Xenon::Backend::BufferType::Index:
				m_pTemporaryBuffer = std::make_unique<VulkanBuffer>(m_pDevice, getSize(), Xenon::Backend::BufferType::Staging);
				usageFlags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
				break;

			case Xenon::Backend::BufferType::Vertex:
				m_pTemporaryBuffer = std::make_unique<VulkanBuffer>(m_pDevice, getSize(), Xenon::Backend::BufferType::Staging);
				usageFlags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
				break;

			case Xenon::Backend::BufferType::Staging:
				usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				vmaFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
				break;

			case Xenon::Backend::BufferType::Storage:
				usageFlags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				vmaFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
				break;

			case Xenon::Backend::BufferType::Uniform:
				usageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				vmaFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
				break;

			case Xenon::Backend::BufferType::Scratch:
				usageFlags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
				memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
				break;

			default:
				m_Type = Xenon::Backend::BufferType::Staging;
				usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
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

			m_pDevice->getAllocator().access([this, createInfo, allocationCreateInfo](VmaAllocator allocator)
				{
					XENON_VK_ASSERT(vmaCreateBuffer(allocator, &createInfo, &allocationCreateInfo, &m_Buffer, &m_Allocation, nullptr), "Failed to create the buffer!");
				}
			);

			// Set the descriptor buffer info.
			m_BufferInfo.buffer = m_Buffer;
			m_BufferInfo.offset = 0;
			m_BufferInfo.range = size;
		}

		VulkanBuffer::VulkanBuffer(VulkanDevice* pDevice, uint64_t size, VkBufferUsageFlags usageFlags, VmaAllocationCreateFlags allocationCreateFlags, VmaMemoryUsage memoryUsage)
			: Buffer(pDevice, size, BufferType::BackendSpecific)
			, VulkanDeviceBoundObject(pDevice)
		{
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
			allocationCreateInfo.flags = allocationCreateFlags;
			allocationCreateInfo.usage = memoryUsage;

			m_pDevice->getAllocator().access([this, createInfo, allocationCreateInfo](VmaAllocator allocator)
				{
					XENON_VK_ASSERT(vmaCreateBuffer(allocator, &createInfo, &allocationCreateInfo, &m_Buffer, &m_Allocation, nullptr), "Failed to create the buffer!");
				}
			);

			// Set the descriptor buffer info.
			m_BufferInfo.buffer = m_Buffer;
			m_BufferInfo.offset = 0;
			m_BufferInfo.range = size;
		}

		VulkanBuffer::~VulkanBuffer()
		{
			// Unmap the buffer if it's already mapped.
			if (m_IsMapped)
				unmap();

			m_pDevice->getAllocator().access([this](VmaAllocator allocator) { vmaDestroyBuffer(allocator, m_Buffer, m_Allocation); });
		}

		void VulkanBuffer::copy(Buffer* pBuffer, uint64_t size, uint64_t srcOffset /*= 0*/, uint64_t dstOffset /*= 0*/)
		{
			OPTICK_EVENT();

			auto commandBuffers = VulkanCommandRecorder(m_pDevice, CommandRecorderUsage::Transfer);
			commandBuffers.begin();
			commandBuffers.copy(pBuffer, srcOffset, this, dstOffset, size);
			commandBuffers.end();
			commandBuffers.submit();
			commandBuffers.wait();
		}

		void VulkanBuffer::write(const std::byte* pData, uint64_t size, uint64_t offset /*= 0*/, CommandRecorder* pCommandRecorder /*= nullptr*/)
		{
			OPTICK_EVENT();

			// Validate the copy size.
			if (size == 0 || size > getSize())
			{
				XENON_LOG_ERROR("Invalid data write size! Write data size: {} Buffer's actual size: {}", size, getSize());
				return;
			}

			// If the buffer is either index of vertex, copy to a staging buffer before writing.
			if (m_Type == BufferType::Index || m_Type == BufferType::Vertex)
			{
				m_pTemporaryBuffer->write(pData, size, offset);

				if (pCommandRecorder)
					pCommandRecorder->as<VulkanCommandRecorder>()->copy(m_pTemporaryBuffer.get(), offset, this, offset, size);

				else
					copy(m_pTemporaryBuffer.get(), size, offset, offset);
			}
			else
			{
				std::copy_n(pData, size, map() + offset);
				unmap();
			}
		}

		const std::byte* VulkanBuffer::beginRead()
		{
			OPTICK_EVENT();

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
			OPTICK_EVENT();

			if (m_Type == BufferType::Index || m_Type == BufferType::Vertex)
				m_pTemporaryBuffer->unmap();

			else
				unmap();
		}

		VkDeviceAddress VulkanBuffer::getDeviceAddress() const
		{
			VkBufferDeviceAddressInfoKHR bufferDeviceAddressInfo = {};
			bufferDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
			bufferDeviceAddressInfo.buffer = m_Buffer;
			return m_pDevice->getDeviceTable().vkGetBufferDeviceAddressKHR(m_pDevice->getLogicalDevice(), &bufferDeviceAddressInfo);
		}

		std::byte* VulkanBuffer::map()
		{
			OPTICK_EVENT();

			// Return if we are mapped already.
			if (m_IsMapped)
				return m_MappedMemory;

			void* pMemory = nullptr;
			m_pDevice->getAllocator().access([this, &pMemory](VmaAllocator allocator) { XENON_VK_ASSERT(vmaMapMemory(allocator, m_Allocation, &pMemory), "Failed to map the staging buffer memory!"); });
			m_MappedMemory = ToBytes(pMemory);

			m_IsMapped = true;
			return m_MappedMemory;
		}

		void VulkanBuffer::unmap()
		{
			OPTICK_EVENT();

			// Return if we are not mapped.
			if (!m_IsMapped)
				return;

			m_pDevice->getAllocator().access([this](VmaAllocator allocator) { vmaUnmapMemory(allocator, m_Allocation); });

			m_MappedMemory = nullptr;
			m_IsMapped = false;
		}
	}
}