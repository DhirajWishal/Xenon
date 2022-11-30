// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/Descriptor.hpp"

#include "DX12DescriptorHeapManager.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * DirectX 12 descriptor class.
		 */
		class DX12Descriptor final : public Descriptor, public DX12DeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param bindingInfo The descriptor's binding information. Make sure that the binding information are in the binding order (the first one is binging 0, second is 1 and so on).
			 * @param type The descriptor type.
			 * @param pManager The descriptor manager pointer.
			 */
			explicit DX12Descriptor(DX12Device* pDevice, const std::vector<DescriptorBindingInfo>& bindingInfo, DescriptorType type, DX12DescriptorHeapManager* pManager);

			/**
			 * Attach a buffer to the descriptor.
			 *
			 * @param binding The binding of the buffer.
			 * @param pBuffer The buffer to attach.
			 */
			void attach(uint32_t binding, Buffer* pBuffer) override;

			/**
			 * Attach an image to the descriptor.
			 *
			 * @param binding The binding of the image.
			 * @param pImage The image to attach.
			 * @param pView The image view.
			 * @param pSampler The image sampler.
			 * @param usage How the image is used in the binding.
			 */
			void attach(uint32_t binding, Image* pImage, ImageView* pView, ImageSampler* pSampler, ImageUsage usage) override;

			/**
			 * Get the CPU CBV, SRV and UAV descriptor heap handle.
			 *
			 * @return The descriptor handle.
			 */
			[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE getCbvSrvUavDescriptorHeapHandleCPU() const;

			/**
			 * Get the GPU CBV, SRV and UAV descriptor heap handle.
			 *
			 * @return The descriptor handle.
			 */
			[[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE getCbvSrvUavDescriptorHeapHandleGPU() const;

			/**
			 * Get the CPU sampler descriptor heap handle.
			 *
			 * @return The descriptor handle.
			 */
			[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE getSamplerDescriptorHeapHandleCPU() const;

			/**
			 * Get the GPU sampler descriptor heap handle.
			 *
			 * @return The descriptor handle.
			 */
			[[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE getSamplerDescriptorHeapHandleGPU() const;

		public:
			DX12DescriptorHeapManager* m_pManager = nullptr;

			UINT m_CbvSrvUavDescriptorHeapBegin = 0;
			UINT m_SamplerDescriptorHeapBegin = 0;
		};
	}
}