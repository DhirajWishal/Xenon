// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DX12Device.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * DirectX 12 buffer class.
		 * This is the base class for all the DirectX 12 buffers.
		 */
		class DX12Buffer
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param size The size of the buffer in bytes.
			 * @param heapType The buffer's heap type.
			 * @param resourceStates The buffer's resource states. Internally it'll automatically set copy source and destination flags.
			 * @param resourceFlags The optional usage resource flags. Default is none.
			 */
			explicit DX12Buffer(DX12Device* pDevice, uint64_t size, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES resourceStates, D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_NONE);

			/**
			 * Default virtual destructor.
			 */
			virtual ~DX12Buffer() = default;

		protected:
			ComPtr<ID3D12Resource> m_Buffer;
			D3D12MA::Allocation* m_pAllocation = nullptr;
		};
	}
}