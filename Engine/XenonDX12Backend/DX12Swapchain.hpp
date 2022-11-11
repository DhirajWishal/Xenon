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

			/**
			 * Present the swapchain to the window.
			 */
			void present() override;

			/**
			 * Recreate the swapchain.
			 * This is called internally when the window is resized or by any other resizing event.
			 */
			void recreate() override;

		private:
			/**
			 * Get the best supported swapchain format.
			 *
			 * @return The best supported format.
			 */
			[[nodiscard]] DXGI_FORMAT getBestSwapchainFormat() const;

		private:
			std::vector<UINT64> m_FenceValues;
			std::vector<ComPtr<ID3D12Resource>> m_SwapchainImages;
			ComPtr<ID3D12DescriptorHeap> m_SwapchainImageHeap;
			ComPtr<IDXGISwapChain3> m_Swapchain;

			ComPtr<ID3D12Fence> m_FrameFence;
			HANDLE m_FenceEvent = nullptr;

			uint32_t m_FrameCount = 0;
			uint32_t m_FrameIndex = 0;

			UINT m_SwapchainImageHeapDescriptorSize = 0;
		};
	}
}