// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12CommandRecorder.hpp"
#include "DX12Macros.hpp"

#include "DX12Buffer.hpp"
#include "DX12Swapchain.hpp"
#include "DX12Rasterizer.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace Xenon
{
	namespace Backend
	{

		DX12CommandRecorder::DX12CommandRecorder(DX12Device* pDevice, CommandRecorderUsage usage, uint32_t bufferCount /*= 1*/)
			:DX12DeviceBoundObject(pDevice)
			, CommandRecorder(pDevice, usage, bufferCount)
			, m_pDevice(pDevice)
		{
			// Create the command lists.
			m_pCommandLists.reserve(bufferCount);
			for (uint32_t i = 0; i < bufferCount; i++)
			{
				// Create the command list.
				ComPtr<ID3D12GraphicsCommandList> commandList;
				XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pDevice->getCommandAllocator(), nullptr, IID_PPV_ARGS(&commandList)), "Failed to create the command list!");

				// Close the 
				XENON_DX12_ASSERT(commandList->Close(), "Failed to stop the current command list!");

				// Insert the created command list.
				m_pCommandLists.emplace_back(std::move(commandList));
			}

			// Select the current command list.
			m_pCurrentCommandList = m_pCommandLists[m_CurrentIndex].Get();
		}

		void DX12CommandRecorder::begin()
		{
			XENON_DX12_ASSERT(m_pDevice->getCommandAllocator()->Reset(), "Failed to reset the command list allocator!");
			XENON_DX12_ASSERT(m_pCurrentCommandList->Reset(m_pDevice->getCommandAllocator(), nullptr), "Failed to reset the command list!");
		}

		void DX12CommandRecorder::copy(Buffer* pSource, uint64_t srcOffset, Buffer* pDestination, uint64_t dstOffset, uint64_t size)
		{
			m_pCurrentCommandList->CopyBufferRegion(pDestination->as<DX12Buffer>()->getResource(), dstOffset, pSource->as<DX12Buffer>()->getResource(), srcOffset, size);
		}

		void DX12CommandRecorder::copy(Image* pSource, Swapchain* pDestination)
		{
			m_pCurrentCommandList->CopyResource(pDestination->as<DX12Swapchain>()->getCurrentSwapchainImageResource(), pSource->as<DX12Image>()->getResource());
		}

		void DX12CommandRecorder::bind(Rasterizer* pRasterizer, const std::vector<Rasterizer::ClearValueType>& clearValues)
		{
			auto dxRasterizer = pRasterizer->as<DX12Rasterizer>();
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(dxRasterizer->getRenderTargetHeap()->GetCPUDescriptorHandleForHeapStart(), dxRasterizer->getFrameIndex(), dxRasterizer->getRenderTargetDescriptorSize());

			m_pCurrentCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
			m_pCurrentCommandList->ClearRenderTargetView(rtvHandle, glm::value_ptr(std::get<glm::vec4>(clearValues.front())), 0, nullptr);

			m_IsRenderTargetBound = true;
		}

		void DX12CommandRecorder::end()
		{
			XENON_DX12_ASSERT(m_pCurrentCommandList->Close(), "Failed to stop the current command list!");
		}

		void DX12CommandRecorder::next()
		{
			m_pCurrentCommandList = m_pCommandLists[incrementIndex()].Get();
		}

		void DX12CommandRecorder::submit(Swapchain* pSawpchain /*= nullptr*/)
		{
			ID3D12CommandList* ppCommandLists[] = { m_pCurrentCommandList };
			m_pDevice->getCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		}

		void DX12CommandRecorder::wait(uint64_t timeout /*= std::numeric_limits<uint64_t>::max()*/)
		{
			ComPtr<ID3D12Fence> fence;
			XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)), "Failed to create the fence!");
			XENON_DX12_ASSERT(m_pDevice->getCommandQueue()->Signal(fence.Get(), 1), "Failed to signal the fence!");

			// Setup synchronization.
			auto fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

			// Validate the event.
			if (fenceEvent == nullptr)
			{
				XENON_LOG_ERROR("Failed to wait till the command list execution!");
				return;
			}

			// Set the event and wait.
			XENON_DX12_ASSERT(fence->SetEventOnCompletion(1, fenceEvent), "Failed to set the fence event on completion event!");
			WaitForSingleObjectEx(fenceEvent, timeout, FALSE);
			CloseHandle(fenceEvent);
		}
	}
}