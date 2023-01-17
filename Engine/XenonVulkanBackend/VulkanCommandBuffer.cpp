// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanCommandBuffer.hpp"
#include "VulkanMacros.hpp"
#include "VulkanSwapchain.hpp"

#include <optick.h>

namespace Xenon
{
	namespace Backend
	{
		VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice* pDevice, VkCommandBuffer buffer, VkCommandPool commandPool, VkPipelineStageFlags stageFlags)
			: VulkanDeviceBoundObject(pDevice)
			, m_CommandBuffer(buffer)
			, m_CommandPool(commandPool)
			, m_StageFlags(stageFlags)
		{
			// Create the fence.
			VkFenceCreateInfo fenceCreateInfo = {};
			fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCreateInfo.flags = 0;
			fenceCreateInfo.pNext = nullptr;

			XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkCreateFence(m_pDevice->getLogicalDevice(), &fenceCreateInfo, nullptr, &m_Fence), "Failed to create fence!");

			VkSemaphoreCreateInfo semaphoreCreateInfo = {};
			semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			semaphoreCreateInfo.pNext = nullptr;
			semaphoreCreateInfo.flags = 0;

			XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkCreateSemaphore(m_pDevice->getLogicalDevice(), &semaphoreCreateInfo, nullptr, &m_SignalSemaphore), "Failed to create the signal semaphore!");
		}

		VulkanCommandBuffer::VulkanCommandBuffer(VulkanCommandBuffer&& other) noexcept
			: VulkanDeviceBoundObject(std::move(other))
			, m_CommandBuffer(std::exchange(other.m_CommandBuffer, VK_NULL_HANDLE))
			, m_CommandPool(std::exchange(other.m_CommandPool, VK_NULL_HANDLE))
			, m_SignalSemaphore(std::exchange(other.m_SignalSemaphore, VK_NULL_HANDLE))
			, m_Fence(std::exchange(other.m_Fence, VK_NULL_HANDLE))
			, m_StageFlags(std::exchange(other.m_StageFlags, 0))
			, m_IsFenceFree(std::exchange(other.m_IsFenceFree, true))
		{
		}

		VulkanCommandBuffer::~VulkanCommandBuffer()
		{
			if (m_pDevice)
			{
				m_pDevice->getDeviceTable().vkFreeCommandBuffers(m_pDevice->getLogicalDevice(), m_CommandPool, 1, &m_CommandBuffer);
				m_pDevice->getDeviceTable().vkDestroyFence(m_pDevice->getLogicalDevice(), m_Fence, nullptr);
				m_pDevice->getDeviceTable().vkDestroySemaphore(m_pDevice->getLogicalDevice(), m_SignalSemaphore, nullptr);
			}
		}

		void VulkanCommandBuffer::wait(uint64_t timeout /*= std::numeric_limits<uint64_t>::max()*/)
		{
			OPTICK_EVENT();

			if (!m_IsFenceFree)
			{
				XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkWaitForFences(m_pDevice->getLogicalDevice(), 1, &m_Fence, VK_TRUE, timeout), "Failed to wait for the fence!");
				XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkResetFences(m_pDevice->getLogicalDevice(), 1, &m_Fence), "Failed to reset fence!");
				m_IsFenceFree = true;
			}
		}

		void VulkanCommandBuffer::submit(VkPipelineStageFlags pipelineStageFlags, VkQueue queue, VulkanSwapchain* pSwapchain /*= nullptr*/)
		{
			OPTICK_EVENT();

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
			if (pSwapchain != nullptr && pSwapchain->isRenderable())
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
			m_SignalSemaphore = std::exchange(other.m_SignalSemaphore, VK_NULL_HANDLE);
			m_Fence = std::exchange(other.m_Fence, VK_NULL_HANDLE);
			m_StageFlags = std::exchange(other.m_StageFlags, 0);
			m_IsFenceFree = std::exchange(other.m_IsFenceFree, true);

			return *this;
		}
	}
}