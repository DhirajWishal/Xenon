// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/VertexBuffer.hpp"

#include "DX12Device.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * DirectX 12 vertex buffer.
		 */
		class DX12VertexBuffer final : public VertexBuffer
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param size The size of the buffer in bytes.
			 * @param stride The byte size of a single vertex entry.
			 */
			explicit DX12VertexBuffer(DX12Device* pDevice, uint64_t size, uint64_t stride);

			/**
			 * Default virtual destructor.
			 */
			~DX12VertexBuffer() override = default;

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
			ComPtr<ID3D12Resource> m_Buffer;
			D3D12_VERTEX_BUFFER_VIEW m_BufferView;
		};
	}
}