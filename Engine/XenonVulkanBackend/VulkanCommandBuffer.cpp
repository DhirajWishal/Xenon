// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanCommandBuffer.hpp"
#include "VulkanMacros.hpp"
#include "VulkanSwapchain.hpp"

namespace Xenon
{
	namespace Backend
	{
		VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice* pDevice, VkCommandBuffer buffer, VkCommandPool commandPool)
			: VulkanDeviceBoundObject(pDevice)
			, m_CommandBuffer(buffer)
			, m_CommandPool(commandPool)
		{
			// Create the fence.
			VkFenceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			createInfo.flags = 0;
			createInfo.pNext = nullptr;

			XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkCreateFence(m_pDevice->getLogicalDevice(), &createInfo, nullptr, &m_Fence), "Failed to create fence!");
		}

		VulkanCommandBuffer::VulkanCommandBuffer(VulkanCommandBuffer&& other) noexcept
			: VulkanDeviceBoundObject(std::move(other))
			, m_CommandBuffer(std::exchange(other.m_CommandBuffer, VK_NULL_HANDLE))
			, m_CommandPool(std::exchange(other.m_CommandPool, VK_NULL_HANDLE))
			, m_WaitSemaphore(std::exchange(other.m_WaitSemaphore, VK_NULL_HANDLE))
			, m_SignalSemaphore(std::exchange(other.m_SignalSemaphore, VK_NULL_HANDLE))
			, m_Fence(std::exchange(other.m_Fence, VK_NULL_HANDLE))
			, m_IsFenceFree(std::exchange(other.m_IsFenceFree, true))
		{
		}

		VulkanCommandBuffer::~VulkanCommandBuffer()
		{
			if (m_pDevice)
			{
				try
				{
					m_pDevice->getInstance()->getDeletionQueue().insert([pDevice = m_pDevice, commandPool = m_CommandPool, buffer = m_CommandBuffer, fence = m_Fence]
						{
							pDevice->getDeviceTable().vkFreeCommandBuffers(pDevice->getLogicalDevice(), commandPool, 1, &buffer);
							pDevice->getDeviceTable().vkDestroyFence(pDevice->getLogicalDevice(), fence, nullptr);
						}
					);
				}
				catch (...)
				{
					XENON_VK_ASSERT(VK_ERROR_UNKNOWN, "Failed to push the command buffer deletion function to the deletion queue!");
				}
			}
		}

		void VulkanCommandBuffer::wait(uint64_t timeout /*= std::numeric_limits<uint64_t>::max()*/)
		{
			if (!m_IsFenceFree)
			{
				XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkWaitForFences(m_pDevice->getLogicalDevice(), 1, &m_Fence, VK_TRUE, timeout), "Failed to wait for the fence!");
				XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkResetFences(m_pDevice->getLogicalDevice(), 1, &m_Fence), "Failed to reset fence!");
				m_IsFenceFree = true;
			}
		}

		void VulkanCommandBuffer::submit(VkPipelineStageFlags pipelineStageFlags, VkQueue queue, VulkanSwapchain* pSwapchain /*= nullptr*/)
		{
			// Create the submit info structure.
			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.waitSemaphoreCount = 0;
			submitInfo.pWaitSemaphores = nullptr;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_CommandBuffer;
			submitInfo.pWaitDstStageMask = &pipelineStageFlags;
			submitInfo.signalSemaphoreCount = 0;
			submitInfo.pSignalSemaphores = nullptr;

			// Get the semaphores from the swapchain if provided.
			if (pSwapchain != nullptr)
			{
				submitInfo.waitSemaphoreCount = 1;
				submitInfo.pWaitSemaphores = pSwapchain->getInFlightSemaphorePtr();

				submitInfo.signalSemaphoreCount = 1;
				submitInfo.pSignalSemaphores = pSwapchain->getRenderFinishedSemaphorePtr();
			}

			// Submit the queue.
			XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkQueueSubmit(queue, 1, &submitInfo, m_Fence), "Failed to submit the queue!");
			m_IsFenceFree = false;
		}

		VulkanCommandBuffer& VulkanCommandBuffer::operator=(VulkanCommandBuffer&& other) noexcept
		{
			VulkanDeviceBoundObject::operator=(std::move(other));

			m_CommandBuffer = std::exchange(other.m_CommandBuffer, VK_NULL_HANDLE);
			m_CommandPool = std::exchange(other.m_CommandPool, VK_NULL_HANDLE);
			m_WaitSemaphore = std::exchange(other.m_WaitSemaphore, VK_NULL_HANDLE);
			m_SignalSemaphore = std::exchange(other.m_SignalSemaphore, VK_NULL_HANDLE);
			m_Fence = std::exchange(other.m_Fence, VK_NULL_HANDLE);
			m_IsFenceFree = std::exchange(other.m_IsFenceFree, true);

			return *this;
		}
	}
}