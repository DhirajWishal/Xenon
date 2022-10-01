// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanVertexBuffer.hpp"
#include "VulkanMacros.hpp"
#include "VulkanCommandBuffers.hpp"

namespace Xenon
{
	namespace Backend
	{
		VulkanVertexBuffer::VulkanVertexBuffer(VulkanDevice* pDevice, uint64_t size, uint64_t stride)
			: VertexBuffer(pDevice, size, stride)
			, VulkanBuffer(pDevice, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE)
		{
		}

		void VulkanVertexBuffer::copy(const Buffer* pBuffer, uint64_t size, uint64_t srcOffset /*= 0*/, uint64_t dstOffset /*= 0*/)
		{
			auto commandBuffers = VulkanCommandBuffers(m_pDevice);
			commandBuffers.begin();
			commandBuffers.copyBuffers(From(pBuffer), srcOffset, this, dstOffset, size);
			commandBuffers.end();
			commandBuffers.submitTransfer();
		}
	}
}