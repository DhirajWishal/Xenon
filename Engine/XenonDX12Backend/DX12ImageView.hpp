// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/ImageView.hpp"

#include "DX12Image.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * DirectX 12 image view class.
		 */
		class DX12ImageView final : public ImageView, public DX12DeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param pImage The image pointer.
			 * @param specification The view specification.
			 */
			explicit DX12ImageView(DX12Device* pDevice, DX12Image* pImage, const ImageViewSpecification& specification);

		private:
			ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap;
		};
	}
}