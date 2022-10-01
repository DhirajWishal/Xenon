// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanCommandBuffer.hpp"

namespace Xenon
{
	namespace Backend
	{
		class VulkanBuffer;

		/**
		 * Vulkan command buffers class.
		 */
		class VulkanCommandBuffers final : public VulkanDeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 * This will create a transfer command buffer which is used for transferring, and other utility purposes.
			 *
			 * @param pDevice The Vulkan device.
			 */
			explicit VulkanCommandBuffers(VulkanDevice* pDevice);

			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The Vulkan device.
			 * @param bufferCount The number of command buffers to have.
			 */
			explicit VulkanCommandBuffers(VulkanDevice* pDevice, uint32_t bufferCount);

			/**
			 * Destructor.
			 */
			~VulkanCommandBuffers() override;

			/**
			 * This will internally select the next command buffer primitive for recording and execution.
			 */
			void next();

			/**
			 * Submit all the graphics commands.
			 *
			 * @param shouldWait Whether we should wait till the commands are executed. Default is true.
			 */
			void submitGraphics(bool shouldWait = true);

			/**
			 * Submit all the compute commands.
			 *
			 * @param shouldWait Whether we should wait till the commands are executed. Default is true.
			 */
			void submitCompute(bool shouldWait = true);

			/**
			 * Submit all the transfer commands.
			 *
			 * @param shouldWait Whether we should wait till the commands are executed. Default is true.
			 */
			void submitTransfer(bool shouldWait = true);

		public:
			/**
			 * Begin the current command buffer recording.
			 */
			void begin();

			/**
			 * Copy data from one buffer to another.
			 *
			 * @param pSourceBuffer The source buffer to copy data from.
			 * @param srcOffset The source buffer's offset to copy from.
			 * @param pDestinationBuffer The destination buffer to copy the data to.
			 * @param dstOffset The destination buffer's offset to copy the data to.
			 * @param size The number of bytes to copy.
			 */
			void copyBuffers(const VulkanBuffer* pSourceBuffer, uint64_t srcOffset, const VulkanBuffer* pDestinationBuffer, uint64_t dstOffset, uint64_t size);

			/**
			 * End the current command buffer recording.
			 */
			void end();

		private:
			std::vector<VulkanCommandBuffer> m_CommandBuffers;

			VkCommandPool m_CommandPool = VK_NULL_HANDLE;
			VulkanCommandBuffer* m_pCurrentCommandBuffer = nullptr;

			uint32_t m_CurrentIndex = 0;
		};
	}
}