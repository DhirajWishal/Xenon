// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/Descriptor.hpp"

#include "DX12DeviceBoundObject.hpp"

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
			 */
			explicit DX12Descriptor(DX12Device* pDevice, const std::vector<DescriptorBindingInfo>& bindingInfo, DescriptorType type);

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

		public:
			std::vector<UINT> m_SamplerIndex;

			std::vector<CD3DX12_DESCRIPTOR_RANGE1> m_Ranges;
			CD3DX12_ROOT_PARAMETER1 m_RootParameter;

			ComPtr<ID3D12DescriptorHeap> m_CbvSrvUavDescriptorHeap;
			ComPtr<ID3D12DescriptorHeap> m_SamplerDescriptorHeap;

			UINT m_CbvSrvUavDescriptorHeapSize = 0;
			UINT m_SamplerDescriptorHeapSize = 0;
		};
	}
}