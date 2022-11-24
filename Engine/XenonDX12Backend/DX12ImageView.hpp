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

			/**
			 * Get the shader resource view description.
			 *
			 * @return The view description.
			 */
			[[nodiscard]] const D3D12_SHADER_RESOURCE_VIEW_DESC& getSRVDescription() const { return m_ShaderResouceView; }

			/**
			 * Get the shader resource view description pointer.
			 *
			 * @return The const view description pointer.
			 */
			[[nodiscard]] const D3D12_SHADER_RESOURCE_VIEW_DESC* getSRVDescriptionPtr() const { return &m_ShaderResouceView; }

			/**
			 * Get the unordered access view description.
			 *
			 * @return The view description.
			 */
			[[nodiscard]] const D3D12_UNORDERED_ACCESS_VIEW_DESC& getUAVDescription() const { return m_UnorderedAccessView; }

			/**
			 * Get the unordered access view description pointer.
			 *
			 * @return The const view description pointer.
			 */
			[[nodiscard]] const D3D12_UNORDERED_ACCESS_VIEW_DESC* getUAVDescriptionPtr() const { return &m_UnorderedAccessView; }

		private:
			D3D12_SHADER_RESOURCE_VIEW_DESC m_ShaderResouceView = {};
			D3D12_UNORDERED_ACCESS_VIEW_DESC m_UnorderedAccessView = {};
		};
	}
}