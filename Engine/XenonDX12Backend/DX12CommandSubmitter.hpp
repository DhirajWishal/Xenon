// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/CommandSubmitter.hpp"

#include "DX12DeviceBoundObject.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * DirectX 12 command submitter class.
		 */
		class DX12CommandSubmitter final : public CommandSubmitter, public DX12DeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 */
			explicit DX12CommandSubmitter(DX12Device* pDevice);

			/**
			 * Destructor.
			 */
			~DX12CommandSubmitter() override;

			/**
			 * Submit the command recorders to the GPU.
			 *
			 * @param pCommandRecorders The command recorders to submit.
			 * @param pSwapchain The swapchain pointer. This is needed when rendering images to a window. Default is nullptr.
			 */
			void submit(const std::vector<CommandRecorder*>& pCommandRecorders, Swapchain* pSwapchain = nullptr) override;

			/**
			 * Submit the command recorders to the GPU.
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
			std::vector<ID3D12CommandList*> m_pCommandLists;

			ComPtr<ID3D12Fence> m_Fence;
		};
	}
}