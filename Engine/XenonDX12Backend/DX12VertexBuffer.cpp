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
		{
			// Create the buffer.
			const auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			const auto resourceDescriptor = CD3DX12_RESOURCE_DESC::Buffer(size);

			XENON_DX12_ASSERT(pDevice->getDevice()->CreateCommittedResource(
				&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&resourceDescriptor,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_Buffer)), "Failed to create the DirectX 12 vertex buffer!");

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