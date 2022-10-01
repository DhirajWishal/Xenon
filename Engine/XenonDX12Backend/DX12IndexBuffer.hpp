// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/IndexBuffer.hpp"

#include "DX12Buffer.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * DirectX 12 index buffer.
		 */
		class DX12IndexBuffer final : public IndexBuffer, public DX12Buffer
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param size The size of the buffer in bytes.
			 * @param indexSize The size of a single index.
			 */
			explicit DX12IndexBuffer(DX12Device* pDevice, uint64_t size, IndexSize indexSize);

			/**
			 * Default destructor.
			 */
			~DX12IndexBuffer() override = default;

			/**
			 * Copy data from another buffer to this buffer.
			 *
			 * @param pBuffer The buffer to copy the data from.
			 * @param size The size in bytes to copy.
			 * @param srcOffset The source buffer's offset. Default is 0.
			 * @param dstOffset The destination buffer's (this) offset. Default is 0.
			 */
			void copy(const Buffer* pBuffer, uint64_t size, uint64_t srcOffset = 0, uint64_t dstOffset = 0) override;

		private:
			/**
			 * Get the DXGI format from the index size.
			 *
			 * @param size The index size.
			 * @return The format.
			 */
			[[nodiscard]] DXGI_FORMAT getFormatFromIndexSize(IndexSize size) const;

		private:
			ComPtr<ID3D12Resource> m_Buffer;
			D3D12_INDEX_BUFFER_VIEW m_BufferView;
		};
	}
}