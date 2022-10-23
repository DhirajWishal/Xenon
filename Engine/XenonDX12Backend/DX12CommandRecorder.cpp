// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12CommandRecorder.hpp"
#include "DX12Macros.hpp"

namespace Xenon
{
	namespace Backend
	{

		DX12CommandRecorder::DX12CommandRecorder(DX12Device* pDevice, CommandRecorderUsage usage, uint32_t bufferCount /*= 1*/)
			: CommandRecorder(pDevice, usage, bufferCount)
			, m_pDevice(pDevice)
		{
			// Create the command lists.
			m_pCommandLists.reserve(bufferCount);
			for (uint32_t i = 0; i < bufferCount; i++)
			{
				// Create the command list.
				ComPtr<ID3D12GraphicsCommandList> commandList;
				XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pDevice->getCommandAllocator(), nullptr, IID_PPV_ARGS(&commandList)), "Failed to create the DirectX 12 command list!");

				// Close the 
				XENON_DX12_ASSERT(commandList->Close(), "Failed to stop the current DirectX 12 command list!");

				// Insert the created command list.
				m_pCommandLists.emplace_back(std::move(commandList));
			}

			// Select the current command list.
			m_pCurrentCommandList = m_pCommandLists[m_CurrentIndex].Get();
		}

		void DX12CommandRecorder::begin()
		{
		}

		void DX12CommandRecorder::end()
		{
			XENON_DX12_ASSERT(m_pCurrentCommandList->Close(), "Failed to stop the current DirectX 12 command list!");
		}

		void DX12CommandRecorder::next()
		{
			m_pCurrentCommandList = m_pCommandLists[incrementIndex()].Get();
		}

		void DX12CommandRecorder::submit()
		{
			ID3D12CommandList* ppCommandLists[] = { m_pCurrentCommandList };
			m_pDevice->getCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		}

		void DX12CommandRecorder::wait(uint64_t timeout /*= std::numeric_limits<uint64_t>::max()*/)
		{
			ComPtr<ID3D12Fence> fence;
			XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)), "Failed to create the DirectX 12 fence!");
			XENON_DX12_ASSERT(m_pDevice->getCommandQueue()->Signal(fence.Get(), 1), "Failed to signal the DirectX 12 fence!");

			// Setup synchronization.
			auto fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

			// Validate the event.
			if (fenceEvent == nullptr)
			{
				XENON_LOG_ERROR("Failed to wait till the DirectX 12 command list execution!");
				return;
			}

			// Set the event and wait.
			XENON_DX12_ASSERT(fence->SetEventOnCompletion(1, fenceEvent), "Failed to set the DirectX 12 fence event on completion event!");
			WaitForSingleObjectEx(fenceEvent, timeout, FALSE);
			CloseHandle(fenceEvent);
		}
	}
}