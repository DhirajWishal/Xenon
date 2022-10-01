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
			 * @param heapProperties The buffer's heap properties.
			 */
			explicit DX12Buffer(DX12Device* pDevice, uint64_t size, CD3DX12_HEAP_PROPERTIES heapProperties);

			/**
			 * Default virtual destructor.
			 */
			virtual ~DX12Buffer() = default;

		protected:
			ComPtr<ID3D12Resource> m_Buffer;
		};
	}
}