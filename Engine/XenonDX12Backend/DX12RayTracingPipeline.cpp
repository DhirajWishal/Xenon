// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12RayTracingPipeline.hpp"
#include "DX12Macros.hpp"
#include "DX12Descriptor.hpp"

#include <spdlog/fmt/xchar.h>
#include <optick.h>

namespace Xenon
{
	namespace Backend
	{
		DX12RayTracingPipeline::DX12RayTracingPipeline(DX12Device* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, const std::vector<ShaderGroup>& shaderGroups, uint32_t maxRayRecursion /*= 4*/)
			: RayTracingPipeline(pDevice, std::move(pCacheHandler), shaderGroups, maxRayRecursion)
			, DX12DescriptorHeapManager(pDevice)
		{
			OPTICK_EVENT();

			auto rayTracingPipeline = CD3DX12_STATE_OBJECT_DESC(D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE);

			// Setup shader groups.
			uint32_t index = 0;
			std::vector<ComPtr<ID3D10Blob>> pShaderBlobs;
			std::vector<std::string> names;
			std::vector<std::wstring> importAndGroupNames;
			for (const auto& group : shaderGroups)
			{
				auto pHitGroup = rayTracingPipeline.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
				pHitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

				const auto& groupName = importAndGroupNames.emplace_back(fmt::format(L"group{}", index));
				pHitGroup->SetHitGroupExport(groupName.c_str());

				if (group.m_RayGenShader.isValid())
				{
					const auto& newName = names.emplace_back(fmt::format("rayGenMain_group{}", index));
					const auto& pBlob = pShaderBlobs.emplace_back(DX12Device::CompileShader(group.m_RayGenShader, ShaderType::RayGen, newName));
					createDXILLibrary(rayTracingPipeline, pBlob.Get());
				}

				if (group.m_IntersectionShader.isValid())
				{
					const auto& newName = names.emplace_back(fmt::format("intersectionMain_group{}", index));
					const auto& pBlob = pShaderBlobs.emplace_back(DX12Device::CompileShader(group.m_IntersectionShader, ShaderType::Intersection, newName));
					createDXILLibrary(rayTracingPipeline, pBlob.Get());

					const auto& wNewName = importAndGroupNames.emplace_back(fmt::format(L"rayGenMain_group{}", index));
					pHitGroup->SetIntersectionShaderImport(wNewName.c_str());
				}

				if (group.m_AnyHitShader.isValid())
				{
					const auto& newName = names.emplace_back(fmt::format("anyHitMain_group{}", index));
					const auto& pBlob = pShaderBlobs.emplace_back(DX12Device::CompileShader(group.m_AnyHitShader, ShaderType::AnyHit, newName));
					createDXILLibrary(rayTracingPipeline, pBlob.Get());

					const auto& wNewName = importAndGroupNames.emplace_back(fmt::format(L"rayGenMain_group{}", index));
					pHitGroup->SetAnyHitShaderImport(wNewName.c_str());
				}

				if (group.m_ClosestHitShader.isValid())
				{
					const auto& newName = names.emplace_back(fmt::format("closestHitMain_group{}", index));
					const auto& pBlob = pShaderBlobs.emplace_back(DX12Device::CompileShader(group.m_ClosestHitShader, ShaderType::ClosestHit, newName));
					createDXILLibrary(rayTracingPipeline, pBlob.Get());

					const auto& wNewName = importAndGroupNames.emplace_back(fmt::format(L"rayGenMain_group{}", index));
					pHitGroup->SetClosestHitShaderImport(wNewName.c_str());
				}

				if (group.m_MissShader.isValid())
				{
					const auto& newName = names.emplace_back(fmt::format("missMain_group{}", index));
					const auto& pBlob = pShaderBlobs.emplace_back(DX12Device::CompileShader(group.m_MissShader, ShaderType::Miss, newName));
					createDXILLibrary(rayTracingPipeline, pBlob.Get());
				}

				if (group.m_CallableShader.isValid())
				{
					const auto& newName = names.emplace_back(fmt::format("callableMain_group{}", index));
					const auto& pBlob = pShaderBlobs.emplace_back(DX12Device::CompileShader(group.m_CallableShader, ShaderType::Callable, newName));
					createDXILLibrary(rayTracingPipeline, pBlob.Get());
				}

				index++;
			}

			// Create the state object.
			XENON_DX12_ASSERT(pDevice->getDevice()->CreateStateObject(rayTracingPipeline, IID_PPV_ARGS(&m_PipelineState)), "Failed to crate the ray tracing state object!");
		}

		std::unique_ptr<Xenon::Backend::Descriptor> DX12RayTracingPipeline::createDescriptor(DescriptorType type)
		{
			OPTICK_EVENT();

			return std::make_unique<DX12Descriptor>(m_pDevice, getBindingInfo(type), type, this);
		}

		void DX12RayTracingPipeline::createDXILLibrary(CD3DX12_STATE_OBJECT_DESC& stateObject, ID3DBlob* pShaderBlob) const
		{
			OPTICK_EVENT();

			auto lib = stateObject.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
			const D3D12_SHADER_BYTECODE libdxil = CD3DX12_SHADER_BYTECODE(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize());
			lib->SetDXILLibrary(&libdxil);
		}
	}
}