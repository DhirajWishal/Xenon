// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanCommandBufferAllocator.hpp"
#include "VulkanMacros.hpp"
#include "VulkanCommandBuffer.hpp"

namespace Xenon
{
	namespace Backend
	{
		VulkanCommandBufferAllocator::VulkanCommandBufferAllocator(VulkanDevice* pDevice, CommandBufferAllocatorUsage usage, uint8_t bufferCount)
			: CommandBufferAllocator(pDevice, usage, bufferCount)
			, VulkanDeviceBoundObject(pDevice)
		{
			// Setup the command pool create info structure and the function which will access the queue..
			auto function = [this](const VulkanQueue& queue)
			{
				VkCommandPoolCreateInfo createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				createInfo.pNext = nullptr;
				createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
				createInfo.queueFamilyIndex = queue.getFamily();

				VkCommandPool commandPool = VK_NULL_HANDLE;
				XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkCreateCommandPool(m_pDevice->getLogicalDevice(), &createInfo, nullptr, &commandPool), "Failed to create the command pool!");
				m_CommandPool = commandPool;
			};

			switch (usage)
			{
			case Xenon::Backend::CommandBufferAllocatorUsage::Compute:
				pDevice->getComputeQueue().access(std::move(function));
				break;

			case Xenon::Backend::CommandBufferAllocatorUsage::Graphics:
				pDevice->getGraphicsQueue().access(std::move(function));
				break;

			case Xenon::Backend::CommandBufferAllocatorUsage::Transfer:
				pDevice->getTransferCommandPool().access(std::move(function));
				break;

			case Xenon::Backend::CommandBufferAllocatorUsage::Secondary:
				pDevice->getGraphicsQueue().access(std::move(function));
				break;

			default:
				XENON_LOG_FATAL("Invalid command buffer allocator usage! Defaulting to Graphics.");
				pDevice->getGraphicsQueue().access(std::move(function));
				m_Usage = CommandBufferAllocatorUsage::Graphics;
				break;
			}

			// Create the allocator.
			VkCommandBufferAllocateInfo allocateInfo = {};
			allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocateInfo.pNext = nullptr;
			allocateInfo.commandPool = m_CommandPool.getUnsafe();
			allocateInfo.level = m_Usage & CommandBufferAllocatorUsage::Secondary ? VK_COMMAND_BUFFER_LEVEL_SECONDARY : VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocateInfo.commandBufferCount = bufferCount;

			auto commandBuffers = std::vector<VkCommandBuffer>(bufferCount);
			XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkAllocateCommandBuffers(m_pDevice->getLogicalDevice(), &allocateInfo, commandBuffers.data()), "Failed to allocate command buffers!");

			// Setup the command buffer objects.
			m_CommandBuffers.reserve(bufferCount);
			for (const auto& buffer : commandBuffers)
				m_CommandBuffers.emplace_back(pDevice, this, buffer);
		}

		VulkanCommandBufferAllocator::~VulkanCommandBufferAllocator()
		{
			m_CommandBuffers.clear();
			m_pDevice->getDeviceTable().vkDestroyCommandPool(m_pDevice->getLogicalDevice(), m_CommandPool.getUnsafe(), nullptr);
		}

		Xenon::Backend::CommandBuffer* VulkanCommandBufferAllocator::getBuffer(uint8_t index)
		{
			return &m_CommandBuffers[index];
		}

		const Xenon::Backend::CommandBuffer* VulkanCommandBufferAllocator::getBuffer(uint8_t index) const
		{
			return &m_CommandBuffers[index];
		}
	}
}