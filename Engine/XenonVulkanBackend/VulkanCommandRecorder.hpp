// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/CommandRecorder.hpp"

#include "VulkanCommandBuffer.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan command recorder class.
		 */
		class VulkanCommandRecorder final : public VulkanDeviceBoundObject, public CommandRecorder
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param usage The command recorder usage.
			 * @param bufferCount The backend primitive buffer count. Default is 1.
			 */
			explicit VulkanCommandRecorder(VulkanDevice* pDevice, CommandRecorderUsage usage, uint32_t bufferCount = 1);

			/**
			 * Destructor.
			 */
			~VulkanCommandRecorder() override = default;

			/**
			 * Set the command recorder state to recording.
			 */
			void begin() override;

			/**
			 * Copy data from one buffer to another.
			 *
			 * @param pSource The source buffer to copy the data from.
			 * @param srcOffset The source buffer offset.
			 * @param pDestination The destination buffer to copy the data to.
			 * @param dstOffse The destination buffer offset.
			 * @param size The amount of data to copy in bytes.
			 */
			void copyBuffer(Buffer* pSource, uint64_t srcOffset, Buffer* pDestination, uint64_t dstOffset, uint64_t size) override;

			/**
			 * End the command recorder recording.
			 */
			void end() override;

			/**
			 * Rotate the command recorder and select the next command buffer primitive.
			 * This is only applicable when having multiple buffers internally.
			 */
			void next() override;

			/**
			 * Submit the recorded commands to the GPU.
			 * 
			 * @param pSwapchain The swapchain pointer. This is needed when rendering images to a window. Default is nullptr.
			 */
			void submit(Swapchain* pSawpchain = nullptr) override;

			/**
			 * Wait till the commands that were recorded has been executed.
			 *
			 * @param timeout The time to wait till the commands are executed. Default is uint64_t max.
			 */
			void wait(uint64_t timeout = std::numeric_limits<uint64_t>::max()) override;

		private:
			std::vector<VulkanCommandBuffer> m_CommandBuffers;
			VulkanCommandBuffer* m_pCurrentBuffer = nullptr;
		};
	}
}