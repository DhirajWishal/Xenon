// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12OcclusionQuery.hpp"
#include "DX12Macros.hpp"

namespace Xenon
{
	namespace Backend
	{
		DX12OcclusionQuery::DX12OcclusionQuery(DX12Device* pDevice, uint64_t sampleCount)
			: OcclusionQuery(pDevice, sampleCount)
			, DX12DeviceBoundObject(pDevice)
		{
			// Create the occlusion query heap.
			D3D12_QUERY_HEAP_DESC heapDesc = {};
			heapDesc.Count = 1;
			heapDesc.Type = D3D12_QUERY_HEAP_TYPE_OCCLUSION;
			XENON_DX12_ASSERT(pDevice->getDevice()->CreateQueryHeap(&heapDesc, IID_PPV_ARGS(&m_QueryHeap)), "Failed to create the occlusion query heap!");
			XENON_DX12_NAME_OBJECT(m_QueryHeap, "Occlusion Query Heap");

			// Create the query buffer.
			D3D12MA::ALLOCATION_DESC allocationDesc = {};
			allocationDesc.HeapType = D3D12_HEAP_TYPE_READBACK;

			const auto queryBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sampleCount * sizeof(uint64_t));
			XENON_DX12_ASSERT(pDevice->getAllocator()->CreateResource(
				&allocationDesc,
				&queryBufferDesc,
				D3D12_RESOURCE_STATE_COPY_DEST,
				nullptr,
				&m_pAllocation,
				IID_NULL,
				nullptr), "Failed to create the occlusion query buffer!");
			XENON_DX12_NAME_OBJECT(getBuffer(), "Occlusion Query Buffer");
		}

		DX12OcclusionQuery::~DX12OcclusionQuery()
		{
			m_pAllocation->Release();
		}
	}
}