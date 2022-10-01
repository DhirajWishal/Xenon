// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12UniformBuffer.hpp"

namespace Xenon
{
	namespace Backend
	{
		DX12UniformBuffer::DX12UniformBuffer(DX12Device* pDevice, uint64_t size)
			: UniformBuffer(pDevice, size)
			, DX12Buffer(pDevice, size, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON)
		{
		}

		void DX12UniformBuffer::copy(const Buffer* pBuffer, uint64_t size, uint64_t srcOffset /*= 0*/, uint64_t dstOffset /*= 0*/)
		{
			copyFrom(From(pBuffer), size, srcOffset, dstOffset);
		}

		void DX12UniformBuffer::write(const std::byte* pData, uint64_t size, uint64_t offset /*= 0*/)
		{
			// TODO: Copy the data to a write only buffer and send it to the GPU (this buffer).
		}

		const std::byte* DX12UniformBuffer::beginRead()
		{
			// TODO: Copy the data to read only buffer and get its memory.
			return nullptr;
		}

		void DX12UniformBuffer::endRead()
		{
			// TODO: Unmap the copy only buffer and delete it.
		}
	}
}