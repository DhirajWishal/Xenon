// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12VertexBuffer.hpp"
#include "DX12Macros.hpp"

namespace Xenon
{
	namespace Backend
	{
		DX12VertexBuffer::DX12VertexBuffer(DX12Device* pDevice, uint64_t size, uint64_t stride)
			: VertexBuffer(pDevice, size, stride)
			, DX12Buffer(pDevice, size, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER)
		{
			// Initialize the buffer view.
			m_BufferView.BufferLocation = m_Buffer->GetGPUVirtualAddress();
			m_BufferView.StrideInBytes = stride;
			m_BufferView.SizeInBytes = size;
		}

		void DX12VertexBuffer::copy(const Buffer* pBuffer, uint64_t size, uint64_t srcOffset /*= 0*/, uint64_t dstOffset /*= 0*/)
		{
			// TODO: Copy the buffer.
		}
	}
}