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
			 */
			void submit() override;

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