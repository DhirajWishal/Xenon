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
			// Create the swapchain.
			DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
			swapChainDesc.BufferCount = 3;	// TODO: Find a better system.
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
			swapchainImageHeapDesc.NumDescriptors = swapChainDesc.BufferCount;
			swapchainImageHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			swapchainImageHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			XENON_DX12_ASSERT(pDevice->getDevice()->CreateDescriptorHeap(&swapchainImageHeapDesc, IID_PPV_ARGS(&m_SwapchainImageHeap)), "Failed to create the swapchain image heap!");

			// Get the heap descriptor size.
			m_SwapchainImageHeapDescriptorSize = pDevice->getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

			// Get the swapchain images.
			CD3DX12_CPU_DESCRIPTOR_HANDLE swapchainImageHeapHandle(m_SwapchainImageHeap->GetCPUDescriptorHandleForHeapStart());

			// Create a Swapchain image for each frame.
			m_SwapchainImages.resize(swapChainDesc.BufferCount);
			for (UINT i = 0; i < swapChainDesc.BufferCount; i++)
			{
				XENON_DX12_ASSERT(m_Swapchain->GetBuffer(i, IID_PPV_ARGS(&m_SwapchainImages[i])), "Failed to get the swapchain back buffer!");
				pDevice->getDevice()->CreateRenderTargetView(m_SwapchainImages[i].Get(), nullptr, swapchainImageHeapHandle);
				swapchainImageHeapHandle.Offset(1, m_SwapchainImageHeapDescriptorSize);
			}
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