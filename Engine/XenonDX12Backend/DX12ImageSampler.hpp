// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/ImageSampler.hpp"

#include "DX12DeviceBoundObject.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * DirectX 12 image sampler class.
		 */
		class DX12ImageSampler final : public ImageSampler, public DX12DeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param specification The sampler specification.
			 */
			explicit DX12ImageSampler(DX12Device* pDevice, const ImageSamplerSpecification& specification);

			/**
			 * Get the sampler description.
			 *
			 * @return The sampler description.
			 */
			[[nodiscard]] const D3D12_SAMPLER_DESC& getSamplerDescription() const { return m_Descriptor; }

			/**
			 * Get the sampler description pointer.
			 *
			 * @return The const sampler description pointer.
			 */
			[[nodiscard]] const D3D12_SAMPLER_DESC* getSamplerDescriptionPtr() const { return &m_Descriptor; }

		private:
			D3D12_SAMPLER_DESC m_Descriptor;
		};
	}
}