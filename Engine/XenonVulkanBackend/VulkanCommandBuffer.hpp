// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDeviceBoundObject.hpp"

#ifdef max
#undef max

#endif

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

			/**
			 * Submit the command buffer to the device.
			 *
			 * @param pipelineStageFlags The pipeline stage flags.
			 * @param queue The queue to submit to.
			 */
			void submit(VkPipelineStageFlags pipelineStageFlags, VkQueue queue);

		public:
			/**
			 * Move assignment operator.
			 *
			 * @param other The other command buffer.
			 * @return The moved buffer reference.
			 */
			VulkanCommandBuffer& operator=(VulkanCommandBuffer&& other) noexcept;

			/**
			 * VkCommandBuffer operator.
			 * This can be used to conveniently get the Vulkan command buffer handle.
			 *
			 * @return The Vulkan command buffer.
			 */
			operator VkCommandBuffer() const { return m_CommandBuffer; }

			/**
			 * VkFence operator.
			 * This can be used to conveniently get the Vulkan fence handle.
			 *
			 * @return The Vulkan fence.
			 */
			operator VkFence() const { return m_Fence; }

		public:
			/**
			 * Get the command buffer.
			 *
			 * @return The command buffer.
			 */
			[[nodiscard]] VkCommandBuffer getCommandBuffer() const { return m_CommandBuffer; }

			/**
			 * Get the command buffer address (pointer).
			 *
			 * @return The const command buffer pointer.
			 */
			[[nodiscard]] const VkCommandBuffer* getCommandBufferAddress() const { return &m_CommandBuffer; }

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