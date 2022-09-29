// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDeviceBoundObject.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan command buffer structure.
		 * This contains the actual Vulkan command buffer and it's synchronization primitives.
		 */
		class VulkanCommandBuffer final : public VulkanDeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer to which the command buffer belongs to.
			 * @param buffer The allocated command buffer.
			 * @param commandPool The command pool to allocate the command buffer from.
			 */
			explicit VulkanCommandBuffer(VulkanDevice* pDevice, VkCommandBuffer buffer, VkCommandPool commandPool);

			/**
			 * Move constructor.
			 *
			 * @param other The other command buffer.
			 */
			VulkanCommandBuffer(VulkanCommandBuffer&& other) noexcept;

			/**
			 * Destructor.
			 */
			~VulkanCommandBuffer() override;

			/**
			 * This will block the thread and wait till the command buffer has finished it's execution.
			 *
			 * @param timeout The timeout time to wait for. Default is the uint64_t max.
			 */
			void wait(uint64_t timeout = std::numeric_limits<uint64_t>::max());

		public:
			/**
			 * Move assignment operator.
			 *
			 * @param other The other command buffer.
			 * @return The moved buffer reference.
			 */
			VulkanCommandBuffer& operator=(VulkanCommandBuffer&& other) noexcept;

		private:
			VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
			VkCommandPool m_CommandPool = VK_NULL_HANDLE;

			VkSemaphore m_WaitSemaphore = VK_NULL_HANDLE;
			VkSemaphore m_SignalSemaphore = VK_NULL_HANDLE;

			VkFence m_Fence = VK_NULL_HANDLE;
			bool m_IsFenceFree = true;
		};
	}
}