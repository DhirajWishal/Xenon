// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanCommandBuffer.hpp"
#include "VulkanMacros.hpp"
#include "VulkanSwapchain.hpp"

#include <optick.h>

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
				if (m_pDevice->getComputeCommandPool().getUnsafe() == m_CommandPool)
					m_pDevice->getComputeCommandPool().access([this](VkCommandPool pool) { m_pDevice->getDeviceTable().vkFreeCommandBuffers(m_pDevice->getLogicalDevice(), pool, 1, &m_CommandBuffer); });

				else if (m_pDevice->getGraphicsCommandPool().getUnsafe() == m_CommandPool)
					m_pDevice->getGraphicsCommandPool().access([this](VkCommandPool pool) { m_pDevice->getDeviceTable().vkFreeCommandBuffers(m_pDevice->getLogicalDevice(), pool, 1, &m_CommandBuffer); });

				else if (m_pDevice->getTransferCommandPool().getUnsafe() == m_CommandPool)
					m_pDevice->getTransferCommandPool().access([this](VkCommandPool pool) { m_pDevice->getDeviceTable().vkFreeCommandBuffers(m_pDevice->getLogicalDevice(), pool, 1, &m_CommandBuffer); });

				m_pDevice->getDeviceTable().vkDestroyFence(m_pDevice->getLogicalDevice(), m_Fence, nullptr);
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
			m_WaitSemaphore = std::exchange(other.m_WaitSemaphore, VK_NULL_HANDLE);
			m_SignalSemaphore = std::exchange(other.m_SignalSemaphore, VK_NULL_HANDLE);
			m_Fence = std::exchange(other.m_Fence, VK_NULL_HANDLE);
			m_IsFenceFree = std::exchange(other.m_IsFenceFree, true);

			return *this;
		}
	}
}