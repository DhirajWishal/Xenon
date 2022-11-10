// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/Swapchain.hpp"

#include "DX12DeviceBoundObject.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * DirectX 12 swapchain class.
		 */
		class DX12Swapchain final : public DX12DeviceBoundObject, public Swapchain
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param title The title of the window.
			 * @param width The window's width.
			 * @param height The window's height.
			 */
			explicit DX12Swapchain(DX12Device* pDevice, const std::string& title, uint32_t width, uint32_t height);

			/**
			 * Destructor.
			 */
			~DX12Swapchain() override = default;

		private:
			/**
			 * Get the best supported swapchain format.
			 *
			 * @return The best supported format.
			 */
			[[nodiscard]] DXGI_FORMAT getBestSwapchainFormat() const;

		private:
			std::vector<ComPtr<ID3D12Resource>> m_SwapchainImages;
			ComPtr<ID3D12DescriptorHeap> m_SwapchainImageHeap;
			ComPtr<IDXGISwapChain3> m_Swapchain;

			UINT m_SwapchainImageHeapDescriptorSize = 0;
		};
	}
}