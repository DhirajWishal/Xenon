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
			 * @param resourceStates The buffer's resource states.
			 * @param resourceFlags The optional usage resource flags. Default is none.
			 */
			explicit DX12Buffer(DX12Device* pDevice, uint64_t size, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES resourceStates, D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_NONE);

			/**
			 * Default virtual destructor.
			 */
			virtual ~DX12Buffer();

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

			/**
			 * Copy data from another buffer to this buffer.
			 *
			 * @param pBuffer The other buffer to copy the data from.
			 * @param size The number of bytes to copy.
			 * @param srcOffset The offset to copy the data from.
			 * @param dstOffset The offset to store the data to (in this buffer).
			 */
			void copyFrom(const DX12Buffer* pBuffer, uint64_t size, uint64_t srcOffset, uint64_t dstOffset);

			/**
			 * Copy the data from a raw pointer to this buffer.
			 *
			 * @param pData The data pointer to copy the data from.
			 * @param size The size of the data to copy in bytes.
			 * @param offset The buffer's offset to copy to. Default is 0.
			 */
			void copyFrom(const std::byte* pData, uint64_t size, uint64_t offset = 0);

			/**
			 * Map the buffer memory to the local address space.
			 *
			 * @return The buffer memory.
			 */
			const std::byte* map();

			/**
			 * Unmap the buffer memory.
			 */
			void unmap();

		public:
			/**
			 * Get the internally managed resource.
			 *
			 * @return The resource pointer.
			 */
			[[nodiscard]] ID3D12Resource* getResource() { return m_pAllocation->GetResource(); }

			/**
			 * Get the internally managed resource.
			 *
			 * @return The const resource pointer.
			 */
			[[nodiscard]] const ID3D12Resource* getResource() const { return m_pAllocation->GetResource(); }

		protected:
			DX12Device* m_pDevice = nullptr;

			D3D12MA::Allocation* m_pAllocation = nullptr;

			std::unique_ptr<DX12Buffer> m_pTemporaryBuffer = nullptr;

		private:
			uint64_t m_Size = 0;
		};
	}
}