// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12CommandSubmitter.hpp"
#include "DX12Macros.hpp"
#include "DX12CommandRecorder.hpp"

namespace Xenon
{
	namespace Backend
	{
		DX12CommandSubmitter::DX12CommandSubmitter(DX12Device* pDevice)
			: CommandSubmitter(pDevice)
			, DX12DeviceBoundObject(pDevice)
			, m_Worker([this] { worker(); })
		{
			XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)), "Failed to create the fence!");
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

		void DX12CommandSubmitter::submit(const std::vector<CommandRecorder*>& pCommandRecorders, Swapchain* pSwapchain /*= nullptr*/)
		{
			wait();

			m_pCommandLists.reserve(m_pCommandLists.size());
			for (const auto& pCommandRecorder : pCommandRecorders)
				m_pCommandLists.emplace_back(pCommandRecorder->as<DX12CommandRecorder>()->getCurrentCommandList());

			m_ConditionVariable.notify_one();
			m_bIsWaiting = true;
		}

		void DX12CommandSubmitter::wait(std::chrono::milliseconds timeout /*= std::chrono::milliseconds(UINT64_MAX)*/)
		{
			const auto nextTimePoint = std::chrono::system_clock::now() + timeout;
			while (nextTimePoint > std::chrono::system_clock::now() && !m_bIsWaiting)
				std::this_thread::sleep_for(std::chrono::microseconds(100));
		}

		void DX12CommandSubmitter::worker()
		{
			auto lock = std::unique_lock(m_Mutex);

			do
			{
				// Wait till the previous command list has been executed.
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

				// Wait till the condition variable is signaled, or if we should terminate or if there are commands to be executed.
				m_ConditionVariable.wait(lock, [this] {return m_bShouldRun == false || m_pCommandLists.empty() == false; });

				// Execute the next command list if there are any.
				if (!m_pCommandLists.empty())
				{
					m_pDevice->getDirectQueue()->ExecuteCommandLists(1, &m_pCommandLists.front());
					XENON_DX12_ASSERT(m_pDevice->getDirectQueue()->Signal(m_Fence.Get(), 1), "Failed to signal the fence!");
					m_pCommandLists.erase(m_pCommandLists.begin());

					m_bIsWaiting = true;
				}

			} while (m_bShouldRun || !m_pCommandLists.empty());
		}
	}
}