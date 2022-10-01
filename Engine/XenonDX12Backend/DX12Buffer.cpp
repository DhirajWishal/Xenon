// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12Buffer.hpp"
#include "DX12Macros.hpp"

namespace Xenon
{
	namespace Backend
	{
		DX12Buffer::DX12Buffer(DX12Device* pDevice, uint64_t size, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES resourceStates, D3D12_RESOURCE_FLAGS resourceFlags /*= D3D12_RESOURCE_FLAG_NONE*/)
		{
			const auto resourceDescriptor = CD3DX12_RESOURCE_DESC::Buffer(size, resourceFlags);

			D3D12MA::ALLOCATION_DESC allocationDesc = {};
			allocationDesc.HeapType = heapType;

			XENON_DX12_ASSERT(pDevice->getAllocator()->CreateResource(
				&allocationDesc,
				&resourceDescriptor,
				D3D12_RESOURCE_STATE_COPY_SOURCE | D3D12_RESOURCE_STATE_COPY_DEST | resourceStates,
				nullptr,
				&m_pAllocation,
				IID_NULL,
				nullptr), "Failed to create the DirectX 12 buffer!");
		}
	}
}