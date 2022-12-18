// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/CommandBufferAllocator.hpp"

#include "VulkanDeviceBoundObject.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan command buffer allocator class.
		 */
		class VulkanCommandBufferAllocator final : public CommandBufferAllocator, public VulkanDeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevcie The device pointer.
			 * @param usage The allocator usage.
			 * @param bufferCount The command buffer count.
			 */
			explicit VulkanCommandBufferAllocator(VulkanDevice* pDevice, CommandBufferAllocatorUsage usage, uint8_t bufferCount);

			/**
			 * Destructor.
			 */
			~VulkanCommandBufferAllocator() override;

			/**
			 * Get the command pool.
			 *
			 * @return The command pool mutex reference.
			 */
			[[nodiscard]] Mutex<VkCommandPool>& getCommandPool() noexcept { return m_CommandPool; }

			/**
			 * Get the command pool.
			 *
			 * @return The command pool mutex reference.
			 */
			[[nodiscard]] const Mutex<VkCommandPool>& getCommandPool() const noexcept { return m_CommandPool; }

		private:
			Mutex<VkCommandPool> m_CommandPool = VK_NULL_HANDLE;
		};
	}
}