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
		class DX12Swapchain final : public Swapchain, public DX12DeviceBoundObject
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
			 * Prepare the swapchain to present.
			 *
			 * @return The swapchain image index.
			 */
			[[nodiscard]] uint32_t prepare() override;

			/**
			 * Present the swapchain to the window.
			 */
			void present() override;

			/**
			 * Recreate the swapchain.
			 * This is called internally when the window is resized or by any other resizing event.
			 */
			void recreate() override;

			/**
			 * Get the current swapchain image resource.
			 *
			 * @return The image resource.
			 */
			[[nodiscard]] ID3D12Resource* getCurrentSwapchainImageResource() { return m_SwapchainImages[m_ImageIndex].Get(); }

			/**
			 * Get the current swapchain image resource.
			 *
			 * @return The image resource.
			 */
			[[nodiscard]] const ID3D12Resource* getCurrentSwapchainImageResource() const { return m_SwapchainImages[m_ImageIndex].Get(); }

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
			ComPtr<IDXGISwapChain3> m_SwapChain;

			ComPtr<ID3D12Fence> m_FrameFence;
			HANDLE m_FenceEvent = nullptr;

			UINT m_SwapchainImageHeapDescriptorSize = 0;
		};
	}
}