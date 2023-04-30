// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/CommandSubmitter.hpp"

#include "VulkanDeviceBoundObject.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan command submitter class.
		 */
		class VulkanCommandSubmitter final : public CommandSubmitter, public VulkanDeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 */
			explicit VulkanCommandSubmitter(VulkanDevice* pDevice);

			/**
			 * Destructor.
			 */
			~VulkanCommandSubmitter() override;

			/**
			 * Submit the command recorders to the GPU.
			 * The dimensions of the vector contains information about batching. Each vector of command recorders (batch) will be executed in parallel (as a batch).
			 * And each batch will wait till the previous batch is executed before starting it's work.
			 *
			 * @param pCommandRecorders The command recorders to submit.
			 * @param pSwapchain The swapchain pointer. This is needed when rendering images to a window. Default is nullptr.
			 */
			void submit(const std::vector<std::vector<Backend::CommandRecorder*>>& pCommandRecorders, Swapchain* pSwapchain = nullptr) override;

			/**
			 * Wait till the commands that were submitted has been executed.
			 *
			 * @param timeout The time to wait till the commands are executed in milliseconds. Default is uint64_t max.
			 */
			void wait(std::chrono::milliseconds timeout = std::chrono::milliseconds(UINT64_MAX)) override;

		private:
			VkFence m_WaitFence = VK_NULL_HANDLE;
		};
	}
}