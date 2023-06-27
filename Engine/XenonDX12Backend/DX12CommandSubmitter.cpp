// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12CommandSubmitter.hpp"
#include "DX12Macros.hpp"
#include "DX12CommandRecorder.hpp"

#include <optick.h>

namespace Xenon
{
	namespace Backend
	{
		DX12CommandSubmitter::DX12CommandSubmitter(DX12Device* pDevice)
			: CommandSubmitter(pDevice)
			, DX12DeviceBoundObject(pDevice)
		{
			XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)), "Failed to create the fence!");
			XENON_DX12_NAME_OBJECT(m_Fence, "Command Submitter Fence");
		}

		DX12CommandSubmitter::~DX12CommandSubmitter()
		{
			try
			{
				wait();
			}
			catch (...)
			{
				XENON_LOG_ERROR("Failed to wait for the command submitter to finish execution!");
			}
		}

		void DX12CommandSubmitter::submit(const std::vector<std::vector<Backend::CommandRecorder*>>& pCommandRecorders, Swapchain* pSwapchain /*= nullptr*/)
		{
			OPTICK_EVENT();

			m_Fence->Signal(0);
			for (UINT i = 0; i < static_cast<UINT>(pCommandRecorders.size()); i++)
			{
				const auto& pCommandRecorder = pCommandRecorders[i];

				std::vector<ID3D12CommandList*> pCommandLists;
				pCommandLists.reserve(pCommandRecorder.size());

				for (const auto pBatchCommandRecorder : pCommandRecorder)
					pCommandLists.emplace_back(pBatchCommandRecorder->as<DX12CommandRecorder>()->getCurrentCommandList());

				m_pDevice->getDirectQueue()->ExecuteCommandLists(static_cast<UINT>(pCommandLists.size()), pCommandLists.data());
				XENON_DX12_ASSERT(m_pDevice->getDirectQueue()->Signal(m_Fence.Get(), i + 1), "Failed to signal the fence!");
			}

			m_bIsWaiting = true;
		}

		void DX12CommandSubmitter::wait(std::chrono::milliseconds timeout /*= std::chrono::milliseconds(UINT64_MAX)*/)
		{
			OPTICK_EVENT();

			if (m_bIsWaiting)
			{
				const auto nextFence = m_Fence->GetCompletedValue() + 1;
				XENON_DX12_ASSERT(m_pDevice->getDirectQueue()->Signal(m_Fence.Get(), nextFence), "Failed to signal the fence!");

				if (m_Fence->GetCompletedValue() < nextFence)
				{
					const auto eventHandle = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);

					// Validate the created event handle.
					if (eventHandle == nullptr)
					{
						XENON_LOG_ERROR("DirectX 12: The created fence event is nullptr!");
						return;
					}

					XENON_DX12_ASSERT(m_Fence->SetEventOnCompletion(nextFence, eventHandle), "Failed to set the event completion handle!");
					WaitForSingleObject(eventHandle, std::numeric_limits<DWORD>::max());
					CloseHandle(eventHandle);
				}

				m_bIsWaiting = false;
			}
		}
	}
}