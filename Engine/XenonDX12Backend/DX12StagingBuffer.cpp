// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12StagingBuffer.hpp"

namespace Xenon
{
	namespace Backend
	{

		DX12StagingBuffer::DX12StagingBuffer(DX12Device* pDevice, uint64_t size)
			: StagingBuffer(pDevice, size)
			, DX12Buffer(pDevice, size, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD))
		{

		}

		void DX12StagingBuffer::copy(const Buffer* pBuffer, uint64_t size, uint64_t srcOffset /*= 0*/, uint64_t dstOffset /*= 0*/)
		{

		}

		std::byte* DX12StagingBuffer::map()
		{
			return nullptr;
		}

		void DX12StagingBuffer::unmap()
		{

		}
	}
}