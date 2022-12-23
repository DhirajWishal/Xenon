// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/Swapchain.hpp"

#include "DX12Image.hpp"

namespace Xenon
{
	namespace Backend
	{
		class DX12RasterizingPipeline;

		/**
		 * DirectX 12 swapchain class.
		 */
		class DX12Swapchain final : public Swapchain, public DX12DeviceBoundObject
		{
			/**
			 * Image to swapchain container.
			 * This structure contains the pipelines and vertex data along with descriptor heaps to copy data from any incoming
			 * texture (with any dimension and format) to the swapchain.
			 */
			struct ImageToSwapchainContainer final
			{
				ComPtr<ID3D12RootSignature> m_RootSignature;
				ComPtr<ID3D12PipelineState> m_PipelineState;
				ComPtr<ID3D12DescriptorHeap> m_CbvSrvUavDescriptorHeap;
				ComPtr<ID3D12DescriptorHeap> m_SamplerDescriptorHeap;
				ComPtr<ID3D12Resource> m_VertexBuffer;
				D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;

				const DX12Image* m_pPreviousColorImage = nullptr;
			};

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

			/**
			 * Get the current CPU descriptor handle.
			 *
			 * @return The descriptor handle.
			 */
			[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE getCPUDescriptorHandle() const;

			/**
			 * Prepare the internal descriptor to copy the incoming image.
			 *
			 * @parm pImage The image to copy.
			 */
			void prepareDescriptorForImageCopy(DX12Image* pImage);

			/**
			 * Get the image to swapchain copy container.
			 *
			 * @return The container.
			 */
			[[nodiscard]] const ImageToSwapchainContainer& getImageToSwapchainCopyContainer() const { return m_ImageCopyContainer; }

		private:
			/**
			 * Get the best supported swapchain format.
			 *
			 * @return The best supported format.
			 */
			[[nodiscard]] DXGI_FORMAT getBestSwapchainFormat() const;

			/**
			 * Setup the image copy container.
			 */
			void setupImageCopyContainer();

		private:
			std::vector<UINT64> m_FenceValues;
			std::vector<ComPtr<ID3D12Resource>> m_SwapchainImages;

			ImageToSwapchainContainer m_ImageCopyContainer;

			ComPtr<ID3D12DescriptorHeap> m_SwapchainImageHeap;
			ComPtr<IDXGISwapChain3> m_SwapChain;

			ComPtr<ID3D12Fence> m_FrameFence;
			HANDLE m_FenceEvent = nullptr;

			DXGI_FORMAT m_SwapChainFormat = DXGI_FORMAT_UNKNOWN;

			UINT m_SwapchainImageHeapDescriptorSize = 0;
		};
	}
}