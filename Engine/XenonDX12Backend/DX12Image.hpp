// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/Image.hpp"

#include "DX12Device.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Direct X 12 image class.
		 */
		class DX12Image final : public Image
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param specification The image specification.
			 * @param heapType The memory heap type. Default is default.
			 * @param resourceStates The image's resource states. Default is copy destination.
			 */
			explicit DX12Image(DX12Device* pDevice, const ImageSpecification& specification, D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATES resourceStates = D3D12_RESOURCE_STATE_COPY_DEST);

			/**
			 * Default destructor.
			 */
			~DX12Image() override;

		private:
			DX12Device* m_pDevice = nullptr;

			D3D12MA::Allocation* m_pAllocation = nullptr;
		};
	}
}