// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CommandRecorder.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Command submitter object.
		 * This object can be used to submit multiple command recorders to the GPU so that they can be executed one after the other.
		 */
		class CommandSubmitter : public BackendObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 */
			explicit CommandSubmitter([[maybe_unused]] const Device* pDevice) {}

			/**
			 * Submit the command recorders to the GPU.
			 * The dimensions of the vector contains information about batching. Each vector of command recorders (batch) will be executed in parallel (as a batch).
			 * And each batch will wait till the previous batch is executed before starting it's work.
			 *
			 * @param pCommandRecorders The command recorders to submit.
			 * @param pSwapchain The swapchain pointer. This is needed when rendering images to a window. Default is nullptr.
			 */
			virtual void submit(const std::vector<std::vector<Backend::CommandRecorder*>>& pCommandRecorders, Swapchain* pSwapchain = nullptr) = 0;

			/**
			 * Wait till the commands that were submitted has been executed.
			 *
			 * @param timeout The time to wait till the commands are executed in milliseconds. Default is uint64_t max.
			 */
			virtual void wait(std::chrono::milliseconds timeout = std::chrono::milliseconds(UINT64_MAX)) = 0;

			/**
			 * Check if the submitter is waiting for execution to complete.
			 *
			 * @return True if all the commands have been executed.
			 * @return False if the commands are still being executed.
			 */
			XENON_NODISCARD bool isWaiting() const noexcept { return m_bIsWaiting; }

		protected:
			bool m_bIsWaiting = false;
		};
	}
}