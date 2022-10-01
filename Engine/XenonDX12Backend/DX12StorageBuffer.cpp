// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12StorageBuffer.hpp"

namespace Xenon
{
	namespace Backend
	{
		DX12StorageBuffer::DX12StorageBuffer(DX12Device* pDevice, uint64_t size)
			: StorageBuffer(pDevice, size)
			, DX12Buffer(pDevice, size, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON)
		{
		}

		void DX12StorageBuffer::copy(const Buffer* pBuffer, uint64_t size, uint64_t srcOffset /*= 0*/, uint64_t dstOffset /*= 0*/)
		{
			copyFrom(From(pBuffer), size, srcOffset, dstOffset);
		}

		void DX12StorageBuffer::write(const std::byte* pData, uint64_t size, uint64_t offset /*= 0*/)
		{
			copyFrom(pData, size, offset);
		}

		const std::byte* DX12StorageBuffer::beginRead()
		{
			return map();
		}

		void DX12StorageBuffer::endRead()
		{
			unmap();
		}
	}
}