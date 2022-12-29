// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12TopLevelAccelerationStructure.hpp"
#include "DX12Macros.hpp"
#include "DX12Buffer.hpp"
#include "DX12CommandRecorder.hpp"
#include "DX12BottomLevelAccelerationStructure.hpp"

#include <DirectXMath.h>

namespace Xenon
{
	namespace Backend
	{
		DX12TopLevelAccelerationStructure::DX12TopLevelAccelerationStructure(DX12Device* pDevice, const std::vector<BottomLevelAccelerationStructure*>& pBottomLevelAccelerationStructures)
			: TopLevelAccelerationStructure(pDevice, pBottomLevelAccelerationStructures)
			, DX12AccelerationStructure(pDevice)
		{
			// Build the instances.
			std::vector<D3D12_RAYTRACING_INSTANCE_DESC> instances;
			instances.reserve(pBottomLevelAccelerationStructures.size());

			for (const auto pAccelerationStructure : pBottomLevelAccelerationStructures)
			{
				auto& instanceDesc = instances.emplace_back();
				instanceDesc = {};
				instanceDesc.InstanceMask = 1;
				instanceDesc.InstanceContributionToHitGroupIndex = 0;
				instanceDesc.AccelerationStructure = pAccelerationStructure->as<DX12BottomLevelAccelerationStructure>()->getAccelerationStructureGPUVirtualAddress();

				const auto transform = DirectX::XMMATRIX(
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
				);

				DirectX::XMStoreFloat3x4(reinterpret_cast<DirectX::XMFLOAT3X4*>(instanceDesc.Transform), transform);
			}

			const auto instanceDataSize = static_cast<UINT64>(instances.size() * sizeof(D3D12_RAYTRACING_INSTANCE_DESC));

			// Create the instance buffer to store the instance data.
			auto instanceBuffer = DX12Buffer(pDevice, instanceDataSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
			instanceBuffer.write(ToBytes(instances.data()), instanceDataSize);

			// Setup the acceleration structure information.
			D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC topLevelBuildDesc = {};
			topLevelBuildDesc.Inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
			topLevelBuildDesc.Inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
			topLevelBuildDesc.Inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
			topLevelBuildDesc.Inputs.NumDescs = static_cast<uint32_t>(pBottomLevelAccelerationStructures.size());

			D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO topLevelPrebuildInfo = {};
			pDevice->getDevice()->GetRaytracingAccelerationStructurePrebuildInfo(&topLevelBuildDesc.Inputs, &topLevelPrebuildInfo);

			// Validate the pre-build info size.
			if (topLevelPrebuildInfo.ResultDataMaxSizeInBytes == 0)
			{
				XENON_LOG_FATAL("The driver does not support acceleration structures!");
				return;
			}

			// Create the scratch buffer.
			createScratchBuffer(topLevelPrebuildInfo.ScratchDataSizeInBytes);
			topLevelBuildDesc.ScratchAccelerationStructureData = m_pScratchBuffer->GetResource()->GetGPUVirtualAddress();

			// Create the acceleration structure.
			createAccelerationStructure(topLevelPrebuildInfo.ResultDataMaxSizeInBytes);
			topLevelBuildDesc.DestAccelerationStructureData = m_pAccelerationStructure->GetResource()->GetGPUVirtualAddress();

			// Setup the virtual addresses.
			topLevelBuildDesc.Inputs.InstanceDescs = instanceBuffer.getResource()->GetGPUVirtualAddress();
			topLevelBuildDesc.ScratchAccelerationStructureData = m_pScratchBuffer->GetResource()->GetGPUVirtualAddress();
			topLevelBuildDesc.DestAccelerationStructureData = m_pAccelerationStructure->GetResource()->GetGPUVirtualAddress();

			// Build the structure.
			auto commandBuffers = DX12CommandRecorder(m_pDevice, CommandRecorderUsage::Graphics);
			commandBuffers.begin();
			commandBuffers.buildAccelerationStructure(topLevelBuildDesc);
			commandBuffers.end();
			commandBuffers.submit();
			commandBuffers.wait();
		}
	}
}