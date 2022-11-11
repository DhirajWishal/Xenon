// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanCommandRecorder.hpp"
#include "VulkanMacros.hpp"

#include "VulkanBuffer.hpp"
#include "VulkanSwapchain.hpp"

namespace Xenon
{
	namespace Backend
	{
		VulkanCommandRecorder::VulkanCommandRecorder(VulkanDevice* pDevice, CommandRecorderUsage usage, uint32_t bufferCount /*= 1*/)
			: VulkanDeviceBoundObject(pDevice)
			, CommandRecorder(pDevice, usage, bufferCount)
		{
			// Get the command pool from the device.
			VkCommandPool commandPool = VK_NULL_HANDLE;
			switch (usage)
			{
			case Xenon::Backend::CommandRecorderUsage::Compute:
				commandPool = pDevice->getComputeCommandPool();
				break;

			case Xenon::Backend::CommandRecorderUsage::Graphics:
				commandPool = pDevice->getGraphicsCommandPool();
				break;

			case Xenon::Backend::CommandRecorderUsage::Transfer:
				commandPool = pDevice->getTransferCommandPool();
				break;

			default:
				XENON_LOG_FATAL("Invalid command recorder usage!");
				return;
			}

			// Allocate the command buffers.
			VkCommandBufferAllocateInfo allocateInfo = {};
			allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocateInfo.pNext = nullptr;
			allocateInfo.commandPool = commandPool;
			allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocateInfo.commandBufferCount = bufferCount;

			std::vector<VkCommandBuffer> commandBuffers(bufferCount);
			XENON_VK_ASSERT(pDevice->getDeviceTable().vkAllocateCommandBuffers(pDevice->getLogicalDevice(), &allocateInfo, commandBuffers.data()), "Failed to allocate command buffers!");

			// Create the command buffers.
			for (uint32_t i = 0; i < bufferCount; i++)
				m_CommandBuffers.emplace_back(pDevice, commandBuffers[i], commandPool);

			// Select the default buffer.
			m_pCurrentBuffer = &m_CommandBuffers[m_CurrentIndex];
		}

		void VulkanCommandRecorder::begin()
		{
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.pNext = VK_NULL_HANDLE;
			beginInfo.flags = 0;
			beginInfo.pInheritanceInfo = nullptr;

			m_pDevice->getDeviceTable().vkBeginCommandBuffer(*m_pCurrentBuffer, &beginInfo);
		}

		void VulkanCommandRecorder::copyBuffer(Buffer* pSource, uint64_t srcOffset, Buffer* pDestination, uint64_t dstOffset, uint64_t size)
		{
			VkBufferCopy copy = {};
			copy.size = size;
			copy.srcOffset = srcOffset;
			copy.dstOffset = dstOffset;

			m_pDevice->getDeviceTable().vkCmdCopyBuffer(*m_pCurrentBuffer, pSource->as<VulkanBuffer>()->getBuffer(), pDestination->as<VulkanBuffer>()->getBuffer(), 1, &copy);
		}

		void VulkanCommandRecorder::end()
		{
			m_pDevice->getDeviceTable().vkEndCommandBuffer(*m_pCurrentBuffer);
		}

		void VulkanCommandRecorder::next()
		{
			m_pCurrentBuffer = &m_CommandBuffers[incrementIndex()];
		}

		void VulkanCommandRecorder::submit(Swapchain* pSawpchain /*= nullptr*/)
		{
			switch (m_Usage)
			{
			case Xenon::Backend::CommandRecorderUsage::Compute:
				m_pCurrentBuffer->submit(VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, m_pDevice->getComputeQueue().getQueue());
				break;

			case Xenon::Backend::CommandRecorderUsage::Graphics:
				m_pCurrentBuffer->submit(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, m_pDevice->getGraphicsQueue().getQueue(), pSawpchain->as<VulkanSwapchain>());
				break;

			case Xenon::Backend::CommandRecorderUsage::Transfer:
				m_pCurrentBuffer->submit(VK_PIPELINE_STAGE_TRANSFER_BIT, m_pDevice->getTransferQueue().getQueue());
				break;

			default:
				XENON_LOG_FATAL("Cannot submit the recorded commands! The command recorder usage used to create the recorder is invalid.");
				break;
			}
		}

		void VulkanCommandRecorder::wait(uint64_t timeout /*= std::numeric_limits<uint64_t>::max()*/)
		{
			m_pCurrentBuffer->wait(timeout);
		}
	}
}