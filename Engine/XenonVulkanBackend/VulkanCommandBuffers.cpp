// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanCommandBuffers.hpp"
#include "VulkanMacros.hpp"

namespace Xenon
{
	namespace Backend
	{
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
			// m_CommandBuffers.reserve(bufferCount);
			for (uint32_t i = 0; i < bufferCount; i++)
				m_CommandBuffers.emplace_back(pDevice, commandBuffers[i], m_CommandPool);
		}

		VulkanCommandBuffers::~VulkanCommandBuffers()
		{
			m_CommandBuffers.clear();
			m_pDevice->getDeviceTable().vkDestroyCommandPool(m_pDevice->getLogicalDevice(), m_CommandPool, nullptr);
		}
	}
}