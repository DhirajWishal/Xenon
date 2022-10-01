// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12IndexBuffer.hpp"
#include "DX12Macros.hpp"

namespace Xenon
{
	namespace Backend
	{
		DX12IndexBuffer::DX12IndexBuffer(DX12Device* pDevice, uint64_t size, IndexSize indexSize)
			: IndexBuffer(pDevice, size, indexSize)
			, DX12Buffer(pDevice, size, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER)
		{
			// Initialize the buffer view.
			m_BufferView.BufferLocation = m_pAllocation->GetResource()->GetGPUVirtualAddress();
			m_BufferView.Format = getFormatFromIndexSize(indexSize);
			m_BufferView.SizeInBytes = size;
		}

		void DX12IndexBuffer::copy(const Buffer* pBuffer, uint64_t size, uint64_t srcOffset /*= 0*/, uint64_t dstOffset /*= 0*/)
		{
			copyFrom(From(pBuffer), size, srcOffset, dstOffset);
		}

		DXGI_FORMAT DX12IndexBuffer::getFormatFromIndexSize(IndexSize size) const
		{
			switch (size)
			{
			case Xenon::IndexSize::Uint8:
				return DXGI_FORMAT_R8_UINT;

			case Xenon::IndexSize::Uint16:
				return DXGI_FORMAT_R16_UINT;

			case Xenon::IndexSize::Uint32:
				return DXGI_FORMAT_R32_UINT;

			default:
				XENON_LOG_ERROR("Invalid index buffer index size!");
				return DXGI_FORMAT_UNKNOWN;
			}
		}
	}
}