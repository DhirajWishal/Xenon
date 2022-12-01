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
		class DX12Rasterizer final : public Rasterizer, public DX12DeviceBoundObject
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
			 * Get the image attachment of the relevant attachment type.
			 *
			 * @param type The attachment type.
			 * @return The attachment image.
			 */
			[[nodiscard]] Image* getImageAttachment(AttachmentType type) override;

			/**
			 * Get the color target heap pointer.
			 *
			 * @return The pointer.
			 */
			[[nodiscard]] ID3D12DescriptorHeap* getColorTargetHeap() { return m_ColorTargetHeap.Get(); }

			/**
			 * Get the color target heap pointer.
			 *
			 * @return The pointer.
			 */
			[[nodiscard]] const ID3D12DescriptorHeap* getColorTargetHeap() const { return m_ColorTargetHeap.Get(); }

			/**
			 * Get the CPU color target heap start.
			 *
			 * @return The pointer.
			 */
			[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE getColorTargetHeapStartCPU();

			/**
			 * Get the CPU color target heap start.
			 *
			 * @return The CPU handle.
			 */
			[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE getColorTargetHeapStartCPU() const;

			/**
			 * Get the depth target heap pointer.
			 *
			 * @return The pointer.
			 */
			[[nodiscard]] ID3D12DescriptorHeap* getDepthTargetHeap() { return m_DepthTargetHeap.Get(); }

			/**
			 * Get the depth target heap pointer.
			 *
			 * @return The pointer.
			 */
			[[nodiscard]] const ID3D12DescriptorHeap* getDepthTargetHeap() const { return m_DepthTargetHeap.Get(); }

			/**
			 * Get the CPU depth target heap start.
			 *
			 * @return The CPU handle.
			 */
			[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE getDepthTargetHeapStartCPU();

			/**
			 * Get the CPU depth target heap start.
			 *
			 * @return The CPU handle.
			 */
			[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE getDepthTargetHeapStartCPU() const;

			/**
			 * Get the color target descriptor size.
			 *
			 * @return The size.
			 */
			[[nodiscard]] UINT getColorTargetDescriptorSize() const { return m_ColorHeapSize; }

			/**
			 * Get the depth target descriptor size.
			 *
			 * @return The size.
			 */
			[[nodiscard]] UINT getDepthTargetDescriptorSize() const { return m_DepthHeapSize; }

			/**
			 * Get the number of color targets stored.
			 *
			 * @return The color target count.
			 */
			[[nodiscard]] uint64_t getColorTargetCount() const noexcept { return m_RenderTargets.size() - (hasTarget(AttachmentType::Depth | AttachmentType::Stencil) ? 1 : 0); }

			/**
			 * Get the render targets.
			 *
			 * @return The render targets.
			 */
			[[nodiscard]] const std::vector<DX12Image>& getRenderTargets() const { return m_RenderTargets; }

			/**
			 * Check if the given attachment type is present in the target.
			 *
			 * @param type The attachment type.
			 * @return True if the attachment type exists.
			 * @return False if the attachment type does not exists.
			 */
			[[nodiscard]] bool hasTarget(AttachmentType type) const noexcept { return m_AttachmentTypes & type; }

		private:
			/**
			 * Get the attachment index for the given type.
			 *
			 * @param type The attachment type.
			 * @return The index.
			 */
			[[nodiscard]] uint8_t getAttachmentIndex(AttachmentType type) const;

			/**
			 * Setup the render target images.
			 */
			void setupRenderTargets();

			/**
			 * Get the best data format from the candidate list.
			 *
			 * @param candidates The candidate formats to choose from.
			 * @param formatSupport The format support to check.
			 * @return The best format.
			 */
			[[nodiscard]] DataFormat getBestFormat(const std::vector<DataFormat>& candidates, D3D12_FORMAT_SUPPORT1 formatSupport) const;

		private:
			std::vector<DX12Image> m_RenderTargets;

			ComPtr<ID3D12DescriptorHeap> m_ColorTargetHeap;
			ComPtr<ID3D12DescriptorHeap> m_ColorShaderViewHeap;

			ComPtr<ID3D12DescriptorHeap> m_DepthTargetHeap;
			ComPtr<ID3D12DescriptorHeap> m_DepthShaderViewHeap;

			UINT m_ColorHeapSize = 0;
			UINT m_DepthHeapSize = 0;
		};
	}
}