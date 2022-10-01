// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DX12Device.hpp"

namespace Xenon
{
	namespace Backend
	{
		class Buffer;

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

			/**
			 * Convert a backend buffer pointer to Vulkan buffer pointer.
			 *
			 * @param pBuffer The buffer pointer.
			 * @return The casted Vulkan buffer pointer.
			 */
			static DX12Buffer* From(Buffer* pBuffer);

			/**
			 * Convert a backend buffer pointer to Vulkan buffer pointer.
			 *
			 * @param pBuffer The buffer pointer.
			 * @return The casted const Vulkan buffer pointer.
			 */
			static const DX12Buffer* From(const Buffer* pBuffer);

		protected:
			/**
			 * Copy data from another buffer to this buffer.
			 *
			 * @param pBuffer The other buffer to copy the data from.
			 * @param size The number of bytes to copy.
			 * @param srcOffset The offset to copy the data from.
			 * @param dstOffset The offset to store the data to (in this buffer).
			 */
			void copyFrom(const DX12Buffer* pBuffer, uint64_t size, uint64_t srcOffset, uint64_t dstOffset);

		protected:
			DX12Device* m_pDevice = nullptr;

			ComPtr<ID3D12Resource> m_Buffer;
			D3D12MA::Allocation* m_pAllocation = nullptr;
		};
	}
}