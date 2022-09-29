// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanIndexBuffer.hpp"
#include "VulkanMacros.hpp"
#include "VulkanCommandBuffers.hpp"

namespace Xenon
{
	namespace Backend
	{
		VulkanIndexBuffer::VulkanIndexBuffer(VulkanDevice* pDevice, uint64_t size, IndexSize indexSize)
			: IndexBuffer(pDevice, size, indexSize)
			, VulkanBuffer(pDevice, size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE)
		{
		}

		VulkanIndexBuffer::~VulkanIndexBuffer()
		{
		}

		void VulkanIndexBuffer::copy(const Buffer* pBuffer, uint64_t size, uint64_t srcOffset /*= 0*/, uint64_t dstOffset /*= 0*/)
		{
			auto commandBuffers = VulkanCommandBuffers(m_pDevice);
			commandBuffers.begin();
			commandBuffers.copyBuffers(pBuffer, srcOffset, this, dstOffset, size);
			commandBuffers.end();
			commandBuffers.submitTransfer();
		}
	}
}