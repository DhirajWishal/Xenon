// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/Rasterizer.hpp"

#include "DX12Device.hpp"
#include "DX12Image.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Direct X 12 rasterizer class.
		 */
		class DX12Rasterizer final : public DX12DeviceBoundObject, public Rasterizer
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param pCamera The camera which is used to render the scene.
			 * @param attachmentTypes The attachment types the render target should support.
			 * @param enableTripleBuffering Whether to enable triple-buffering. Default is false.
			 * @param multiSampleCount Multi-sampling count to use. Default is x1.
			 */
			explicit DX12Rasterizer(DX12Device* pDevice, Camera* pCamera, AttachmentType attachmentTypes, bool enableTripleBuffering = false, MultiSamplingCount multiSampleCount = MultiSamplingCount::x1);

			/**
			 * Destructor.
			 */
			~DX12Rasterizer() override = default;

			/**
			 * Get the frame index.
			 *
			 * @return The frame index.
			 */
			[[nodiscard]] uint32_t getFrameIndex() const { return m_FrameIndex; }

			/**
			 * Get the render target heap pointer.
			 *
			 * @return The pointer.
			 */
			[[nodiscard]] ID3D12DescriptorHeap* getRenderTargetHeap() { return m_RenderTargetHeap.Get(); }

			/**
			 * Get the render target heap pointer.
			 *
			 * @return The pointer.
			 */
			[[nodiscard]] const ID3D12DescriptorHeap* getRenderTargetHeap() const { return m_RenderTargetHeap.Get(); }

			/**
			 * Get the render target descriptor size.
			 *
			 * @return The size.
			 */
			[[nodiscard]] UINT getRenderTargetDescriptorSize() const { return m_RenderTargetDescriptorSize; }

		private:
			std::vector<DX12Image> m_RenderTargets;
			ComPtr<ID3D12DescriptorHeap> m_RenderTargetHeap;
			ComPtr<ID3D12DescriptorHeap> m_RenderTargetShaderViewHeap;

			UINT m_RenderTargetDescriptorSize = 0;

			DX12Device* m_pDevice = nullptr;

			uint32_t m_FrameIndex = 0;
		};
	}
}