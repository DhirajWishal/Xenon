// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12OcclusionQuery.hpp"
#include "DX12Macros.hpp"

#ifdef XENON_PLATFORM_WINDOWS
#include <execution> 

#endif // XENON_PLATFORM_WINDOWS


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

			// Create the query buffer.
			D3D12MA::ALLOCATION_DESC allocationDesc = {};
			allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

			const auto queryBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sampleCount * sizeof(uint64_t));
			XENON_DX12_ASSERT(pDevice->getAllocator()->CreateResource(
				&allocationDesc,
				&queryBufferDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				&m_pAllocation,
				IID_NULL,
				nullptr), "Failed to create the occlusion query buffer!");
		}

		DX12OcclusionQuery::~DX12OcclusionQuery()
		{
			m_pAllocation->Release();
		}

		const std::vector<uint64_t>& DX12OcclusionQuery::getResults()
		{
			const D3D12_RANGE mapRange = CD3DX12_RANGE(1, 0);

			uint64_t* pSampleData = nullptr;
			XENON_DX12_ASSERT(m_pAllocation->GetResource()->Map(0, nullptr, std::bit_cast<void**>(&pSampleData)), "Failed to map the occlusion query buffer!");
			m_pAllocation->GetResource()->Unmap(0, &mapRange);

#ifdef XENON_PLATFORM_WINDOWS
			std::copy_n(std::execution::unseq, pSampleData, m_Samples.size(), m_Samples.data());

#else
			std::copy_n(pSampleData, m_Samples.size(), m_Samples.data());

#endif // XENON_PLATFORM_WINDOWS

			return m_Samples;
		}
	}
}