// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12StagingBuffer.hpp"

namespace Xenon
{
	namespace Backend
	{

		DX12StagingBuffer::DX12StagingBuffer(DX12Device* pDevice, uint64_t size)
			: StagingBuffer(pDevice, size)
			, DX12Buffer(pDevice, size, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_COMMON)
		{

		}

		void DX12StagingBuffer::copy(const Buffer* pBuffer, uint64_t size, uint64_t srcOffset /*= 0*/, uint64_t dstOffset /*= 0*/)
		{

		}

		void DX12StagingBuffer::write(const std::byte* pData, uint64_t size, uint64_t offset /*= 0*/)
		{
			// TODO: Copy the data to a write only buffer and send it to the GPU (this buffer).
		}

		const std::byte* DX12StagingBuffer::beginRead()
		{
			// TODO: Copy the data to read only buffer and get its memory.
			return nullptr;
		}

		void DX12StagingBuffer::endRead()
		{
			// TODO: Unmap the copy only buffer and delete it.
		}
	}
}