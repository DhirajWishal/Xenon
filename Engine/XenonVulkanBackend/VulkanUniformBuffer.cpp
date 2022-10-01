// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanUniformBuffer.hpp"
#include "VulkanMacros.hpp"

namespace Xenon
{
	namespace Backend
	{
		VulkanUniformBuffer::VulkanUniformBuffer(VulkanDevice* pDevice, uint64_t size)
			: UniformBuffer(pDevice, size)
			, VulkanBuffer(pDevice, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST)
		{
		}

		void VulkanUniformBuffer::copy(const Buffer* pBuffer, uint64_t size, uint64_t srcOffset /*= 0*/, uint64_t dstOffset /*= 0*/)
		{
			copyFrom(From(pBuffer), size, srcOffset, dstOffset);
		}

		void VulkanUniformBuffer::write(const std::byte* pData, uint64_t size, uint64_t offset /*= 0*/)
		{
			void* pDataStore = nullptr;
			XENON_VK_ASSERT(vmaMapMemory(m_pDevice->getAllocator(), m_Allocation, &pDataStore), "Failed to map the staging buffer memory!");

			std::copy_n(pData, size, reinterpret_cast<std::byte*>(pDataStore));
			endRead();
		}

		const std::byte* VulkanUniformBuffer::beginRead()
		{
			void* pDataStore = nullptr;

			XENON_VK_ASSERT(vmaMapMemory(m_pDevice->getAllocator(), m_Allocation, &pDataStore), "Failed to map the staging buffer memory!");
			return reinterpret_cast<std::byte*>(pDataStore);
		}

		void VulkanUniformBuffer::endRead()
		{
			vmaUnmapMemory(m_pDevice->getAllocator(), m_Allocation);
		}
	}
}