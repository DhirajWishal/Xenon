// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12AccelerationStructure.hpp"
#include "DX12Macros.hpp"

namespace Xenon
{
	namespace Backend
	{
		DX12AccelerationStructure::DX12AccelerationStructure(DX12Device* pDevice)
			: DX12DeviceBoundObject(pDevice)
		{
		}

		DX12AccelerationStructure::~DX12AccelerationStructure()
		{
			m_pScratchBuffer->Release();
			m_pAccelerationStructure->Release();
		}

		void DX12AccelerationStructure::createScratchBuffer(UINT64 size)
		{
			D3D12MA::ALLOCATION_DESC allocationDesc = {};
			allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

			CD3DX12_RESOURCE_DESC resourceDescriptor = CD3DX12_RESOURCE_DESC::Buffer(size, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
			XENON_DX12_ASSERT(m_pDevice->getAllocator()->CreateResource(
				&allocationDesc,
				&resourceDescriptor,
				D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
				nullptr,
				&m_pScratchBuffer,
				IID_NULL,
				nullptr), "Failed to create the scratch buffer!");

			XENON_DX12_NAME_OBJECT(m_pScratchBuffer, "ScratchBuffer");
		}

		void DX12AccelerationStructure::createAccelerationStructure(UINT64 size)
		{
			D3D12MA::ALLOCATION_DESC allocationDesc = {};
			allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

			CD3DX12_RESOURCE_DESC resourceDescriptor = CD3DX12_RESOURCE_DESC::Buffer(size, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
			XENON_DX12_ASSERT(m_pDevice->getAllocator()->CreateResource(
				&allocationDesc,
				&resourceDescriptor,
				D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
				nullptr,
				&m_pAccelerationStructure,
				IID_NULL,
				nullptr), "Failed to create the acceleration structure!");

			XENON_DX12_NAME_OBJECT(m_pAccelerationStructure, "AccelerationStructure");
			m_ResultDataMaxSizeInBytes = size;
		}
	}
}