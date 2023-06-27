// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12OcclusionQuery.hpp"
#include "DX12Macros.hpp"

#include <optick.h>

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
			heapDesc.Count = static_cast<UINT>(sampleCount);
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

		std::vector<uint64_t> DX12OcclusionQuery::getSamples()
		{
			OPTICK_EVENT();

			// Copy the available data.
			const D3D12_RANGE mapRange = CD3DX12_RANGE(1, 0);

			uint64_t* pSampleData = nullptr;
			XENON_DX12_ASSERT(getBuffer()->Map(0, nullptr, XENON_BIT_CAST(void**, &pSampleData)), "Failed to map the occlusion query buffer!");
			getBuffer()->Unmap(0, &mapRange);

			auto samples = std::vector<uint64_t>(m_SampleCount);
#ifdef XENON_PLATFORM_WINDOWS
			std::copy_n(std::execution::unseq, pSampleData, getSampleCount(), samples.data());

#else
			std::copy_n(pSampleData, getSampleCount(), samples.data());

#endif // XENON_PLATFORM_WINDOWS

			return samples;
		}
	}
}