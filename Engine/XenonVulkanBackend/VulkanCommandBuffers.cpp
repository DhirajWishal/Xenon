// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanCommandBuffers.hpp"
#include "VulkanMacros.hpp"

#include "VulkanIndexBuffer.hpp"
#include "VulkanStagingBuffer.hpp"
#include "VulkanStorageBuffer.hpp"
#include "VulkanUniformBuffer.hpp"
#include "VulkanVertexBuffer.hpp"

namespace Xenon
{
	namespace Backend
	{
		VulkanCommandBuffers::VulkanCommandBuffers(VulkanDevice* pDevice)
			: CommandBuffers(pDevice, 1)
			, VulkanDeviceBoundObject(pDevice)
		{
			// Create the command pool.
			VkCommandPoolCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			createInfo.queueFamilyIndex = pDevice->getTransferQueue().getFamily();

			XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkCreateCommandPool(m_pDevice->getLogicalDevice(), &createInfo, nullptr, &m_CommandPool), "Failed to create the command pool!");

			// Allocate the command buffers.
			VkCommandBufferAllocateInfo allocateInfo = {};
			allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocateInfo.pNext = nullptr;
			allocateInfo.commandPool = m_CommandPool;
			allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocateInfo.commandBufferCount = 1;

			VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
			XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkAllocateCommandBuffers(m_pDevice->getLogicalDevice(), &allocateInfo, &commandBuffer), "Failed to allocate command buffers!");

			// Populate the vector.
			m_CommandBuffers.emplace_back(pDevice, commandBuffer, m_CommandPool);

			// Get the first command buffer as the current command buffer.
			m_pCurrentCommandBuffer = &m_CommandBuffers[m_CurrentIndex];
		}

		VulkanCommandBuffers::VulkanCommandBuffers(VulkanDevice* pDevice, uint32_t bufferCount)
			: CommandBuffers(pDevice, bufferCount)
			, VulkanDeviceBoundObject(pDevice)
		{
			// Create the command pool.
			VkCommandPoolCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			createInfo.queueFamilyIndex = pDevice->getGraphicsQueue().getFamily();

			XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkCreateCommandPool(m_pDevice->getLogicalDevice(), &createInfo, nullptr, &m_CommandPool), "Failed to create the command pool!");

			// Allocate the command buffers.
			VkCommandBufferAllocateInfo allocateInfo = {};
			allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocateInfo.pNext = nullptr;
			allocateInfo.commandPool = m_CommandPool;
			allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocateInfo.commandBufferCount = bufferCount;

			std::vector<VkCommandBuffer> commandBuffers(bufferCount);
			XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkAllocateCommandBuffers(m_pDevice->getLogicalDevice(), &allocateInfo, commandBuffers.data()), "Failed to allocate command buffers!");

			// Populate the vector.
			m_CommandBuffers.reserve(bufferCount);
			for (uint32_t i = 0; i < bufferCount; i++)
				m_CommandBuffers.emplace_back(pDevice, commandBuffers[i], m_CommandPool);

			// Get the first command buffer as the current command buffer.
			m_pCurrentCommandBuffer = &m_CommandBuffers[m_CurrentIndex];
		}

		VulkanCommandBuffers::~VulkanCommandBuffers()
		{
			m_CommandBuffers.clear();
			m_pDevice->getDeviceTable().vkDestroyCommandPool(m_pDevice->getLogicalDevice(), m_CommandPool, nullptr);
		}

		void VulkanCommandBuffers::next()
		{
			if (++m_CurrentIndex == m_CommandBuffers.size())
				m_CurrentIndex = 0;

			m_pCurrentCommandBuffer = &m_CommandBuffers[m_CurrentIndex];
		}

		void VulkanCommandBuffers::submitGraphics(bool shouldWait /*= true*/)
		{
			m_pCurrentCommandBuffer->submit(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, m_pDevice->getGraphicsQueue().getQueue());

			if (shouldWait)
				m_pCurrentCommandBuffer->wait();
		}

		void VulkanCommandBuffers::submitCompute(bool shouldWait /*= true*/)
		{
			m_pCurrentCommandBuffer->submit(VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, m_pDevice->getComputeQueue().getQueue());

			if (shouldWait)
				m_pCurrentCommandBuffer->wait();
		}

		void VulkanCommandBuffers::submitTransfer(bool shouldWait /*= true*/)
		{
			m_pCurrentCommandBuffer->submit(VK_PIPELINE_STAGE_TRANSFER_BIT, m_pDevice->getTransferQueue().getQueue());

			if (shouldWait)
				m_pCurrentCommandBuffer->wait();
		}

		void VulkanCommandBuffers::begin()
		{
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.pNext = nullptr;
			beginInfo.flags = 0;
			beginInfo.pInheritanceInfo = nullptr;

			m_pDevice->getDeviceTable().vkBeginCommandBuffer(m_pCurrentCommandBuffer->getCommandBuffer(), &beginInfo);
		}

		void VulkanCommandBuffers::copyBuffers(const Buffer* pSourceBuffer, uint64_t srcOffset, const Buffer* pDestinationBuffer, uint64_t dstOffset, uint64_t size)
		{
			VkBufferCopy copy = {};
			copy.size = size;
			copy.dstOffset = dstOffset;
			copy.srcOffset = srcOffset;

			m_pDevice->getDeviceTable().vkCmdCopyBuffer(m_pCurrentCommandBuffer->getCommandBuffer(), getBufferHandle(pSourceBuffer), getBufferHandle(pDestinationBuffer), 1, &copy);
		}

		void VulkanCommandBuffers::end()
		{
			m_pDevice->getDeviceTable().vkEndCommandBuffer(m_pCurrentCommandBuffer->getCommandBuffer());
		}

		VkBuffer VulkanCommandBuffers::getBufferHandle(const Buffer* pBuffer) const
		{
			switch (pBuffer->getType())
			{
			case Xenon::Backend::BufferType::Index:
				return pBuffer->as<VulkanIndexBuffer>()->getBuffer();

			case Xenon::Backend::BufferType::Vertex:
				return pBuffer->as<VulkanVertexBuffer>()->getBuffer();

			case Xenon::Backend::BufferType::Staging:
				return pBuffer->as<VulkanStagingBuffer>()->getBuffer();

			case Xenon::Backend::BufferType::Storage:
				return pBuffer->as<VulkanStorageBuffer>()->getBuffer();

			case Xenon::Backend::BufferType::Uniform:
				return pBuffer->as<VulkanUniformBuffer>()->getBuffer();

			default:
				XENON_LOG_ERROR("Invalid buffer type!");
				return VK_NULL_HANDLE;
			}
		}
	}
}