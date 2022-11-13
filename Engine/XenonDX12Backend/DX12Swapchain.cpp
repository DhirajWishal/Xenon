// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12Swapchain.hpp"
#include "DX12Macros.hpp"

#include "../XenonPlatformWindows/WindowsWindow.hpp"

namespace Xenon
{
	namespace Backend
	{
		DX12Swapchain::DX12Swapchain(DX12Device* pDevice, const std::string& title, uint32_t width, uint32_t height)
			: DX12DeviceBoundObject(pDevice)
			, Swapchain(pDevice, title, width, height)
		{
			m_FrameCount = 3;	// TODO: Find a better system.

			// Create the swapchain.
			DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
			swapChainDesc.BufferCount = m_FrameCount;
			swapChainDesc.Width = width;
			swapChainDesc.Height = height;
			swapChainDesc.Format = getBestSwapchainFormat();
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swapChainDesc.SampleDesc.Count = 1;

			const auto windowHandle = static_cast<Platform::WindowsWindow*>(m_pWindow.get())->getWindowHandle();
			ComPtr<IDXGISwapChain1> swapChain;
			XENON_DX12_ASSERT(pDevice->getFactory()->CreateSwapChainForHwnd(
				pDevice->getCommandQueue(),
				windowHandle,
				&swapChainDesc,
				nullptr,
				nullptr,
				&swapChain
			), "Failed to create the swapchain!");

			// This sample does not support full screen transitions.
			XENON_DX12_ASSERT(pDevice->getFactory()->MakeWindowAssociation(windowHandle, DXGI_MWA_NO_ALT_ENTER), "Failed to make the window association!");

			XENON_DX12_ASSERT(swapChain.As(&m_Swapchain), "Failed to assign the swapchain!");

			// Create the swapchain image heap.
			D3D12_DESCRIPTOR_HEAP_DESC swapchainImageHeapDesc = {};
			swapchainImageHeapDesc.NumDescriptors = m_FrameCount;
			swapchainImageHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			swapchainImageHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			XENON_DX12_ASSERT(pDevice->getDevice()->CreateDescriptorHeap(&swapchainImageHeapDesc, IID_PPV_ARGS(&m_SwapchainImageHeap)), "Failed to create the swapchain image heap!");

			// Get the heap descriptor size.
			m_SwapchainImageHeapDescriptorSize = pDevice->getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

			// Get the swapchain images.
			CD3DX12_CPU_DESCRIPTOR_HANDLE swapchainImageHeapHandle(m_SwapchainImageHeap->GetCPUDescriptorHandleForHeapStart());

			// Create a Swapchain image for each frame.
			m_SwapchainImages.resize(m_FrameCount);
			for (UINT i = 0; i < m_FrameCount; i++)
			{
				XENON_DX12_ASSERT(m_Swapchain->GetBuffer(i, IID_PPV_ARGS(&m_SwapchainImages[i])), "Failed to get the swapchain back buffer!");
				pDevice->getDevice()->CreateRenderTargetView(m_SwapchainImages[i].Get(), nullptr, swapchainImageHeapHandle);
				swapchainImageHeapHandle.Offset(1, m_SwapchainImageHeapDescriptorSize);
			}

			// Create the fence.
			XENON_DX12_ASSERT(pDevice->getDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_FrameFence)), "Failed to create the frame fence!");
			m_FenceValues.resize(m_FrameCount);
		}

		uint32_t DX12Swapchain::prepare()
		{
			return m_ImageIndex;
		}

		void DX12Swapchain::present()
		{
			// Present the swapchain.
			XENON_DX12_ASSERT(m_Swapchain->Present(1, 0), "Failed to present the swapchain!");

			// Move to the next frame.
			// Schedule a Signal command in the queue.
			const UINT64 currentFenceValue = m_FenceValues[m_ImageIndex];
			XENON_DX12_ASSERT(m_pDevice->getCommandQueue()->Signal(m_FrameFence.Get(), currentFenceValue), "Failed to signal the command queue!");

			// Update the frame index.
			m_ImageIndex = m_Swapchain->GetCurrentBackBufferIndex();

			// If the next frame is not ready to be rendered yet, wait until it is ready.
			if (m_FrameFence->GetCompletedValue() < m_FenceValues[m_ImageIndex])
			{
				XENON_DX12_ASSERT(m_FrameFence->SetEventOnCompletion(m_FenceValues[m_ImageIndex], m_FenceEvent), "Failed t set the event on completion to the fence event!");
				WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);
			}

			// Set the fence value for the next frame.
			m_FenceValues[m_ImageIndex] = currentFenceValue + 1;

			// Increment the frame index.
			incrementFrame();
		}

		void DX12Swapchain::recreate()
		{
			// TODO: Implement this function.
		}

		DXGI_FORMAT DX12Swapchain::getBestSwapchainFormat() const
		{
			const std::array<DXGI_FORMAT, 8> candidates = {
				DXGI_FORMAT_R16G16B16A16_FLOAT,
				DXGI_FORMAT_R10G10B10A2_UNORM,
				DXGI_FORMAT_R8G8B8A8_UNORM,
				DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
				DXGI_FORMAT_B8G8R8A8_UNORM,
				DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
				DXGI_FORMAT_NV12,
				DXGI_FORMAT_YUY2
			};

			// Pick the best candidate.
			for (const auto candidate : candidates)
			{
				if (m_pDevice->getFormatSupport(candidate, D3D12_FORMAT_SUPPORT1_RENDER_TARGET | D3D12_FORMAT_SUPPORT1_DISPLAY).first)
					return candidate;
			}

			XENON_LOG_FATAL("No suitable swapchain formats found!");
			return DXGI_FORMAT_UNKNOWN;
		}
	}
}