// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12Buffer.hpp"
#include "DX12Macros.hpp"

namespace Xenon
{
	namespace Backend
	{
		DX12Buffer::DX12Buffer(DX12Device* pDevice, uint64_t size, CD3DX12_HEAP_PROPERTIES heapProperties)
		{
			const auto resourceDescriptor = CD3DX12_RESOURCE_DESC::Buffer(size);
			XENON_DX12_ASSERT(pDevice->getDevice()->CreateCommittedResource(
				&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&resourceDescriptor,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_Buffer)), "Failed to create the DirectX 12 buffer!");
		}
	}
}