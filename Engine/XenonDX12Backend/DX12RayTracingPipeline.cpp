// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12RayTracingPipeline.hpp"
#include "DX12Macros.hpp"
#include "DX12Descriptor.hpp"
#include "DX12ShaderBindingTable.hpp"

#include <spdlog/fmt/xchar.h>
#include <optick.h>

#include <algorithm>

namespace /* anonymous */
{
	/**
	 * Setup all the shader-specific data.
	 *
	 * @param shader The shader to get the data from.
	 * @param rangeMap The descriptor range map.
	 * @param type The shader's type.
	 */
	void SetupShaderData(
		const Xenon::Backend::Shader& shader,
		std::unordered_map<Xenon::Backend::DescriptorType, std::unordered_map<uint32_t, Xenon::Backend::DescriptorBindingInfo>>& bindingMap,
		std::unordered_map<uint8_t, std::vector<CD3DX12_DESCRIPTOR_RANGE1>>& rangeMap,
		Xenon::Backend::ShaderType type)
	{
		// Setup resources.
		for (const auto& resource : shader.getResources())
		{
			// Fill up the binding info structure.
			auto& bindings = bindingMap[static_cast<Xenon::Backend::DescriptorType>(Xenon::EnumToInt(resource.m_Set))];

			if (bindings.contains(resource.m_Binding))
			{
				bindings[resource.m_Binding].m_ApplicableShaders |= type;
			}
			else
			{
				auto& binding = bindings[resource.m_Binding];
				binding.m_Type = resource.m_Type;
				binding.m_ApplicableShaders |= type;
			}

			// Setup the ranges.
			const auto rangeType = Xenon::Backend::DX12Device::GetDescriptorRangeType(resource.m_Type, resource.m_Operations);
			const auto setIndex = static_cast<uint8_t>(Xenon::EnumToInt(resource.m_Set) * 2);

			// If it's a sampler, we need one for the texture (SRV) and another as the sampler.
			if (rangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER)
			{
				rangeMap[setIndex + 0].emplace_back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, resource.m_Binding, Xenon::EnumToInt(resource.m_Set));	// Set the texture buffer (SRV).
				rangeMap[setIndex + 1].emplace_back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, resource.m_Binding, Xenon::EnumToInt(resource.m_Set));	// Set the texture sampler.
			}

			// Else just one entry for the buffer.
			else
			{
				rangeMap[setIndex].emplace_back().Init(rangeType, 1, resource.m_Binding, Xenon::EnumToInt(resource.m_Set));
			}
		}
	}

	/**
	 * Get the descriptor type bit.
	 * This converts a descriptor type to it's unique bit.
	 *
	 * @param type The descriptor type.
	 * @return The descriptor type bit.
	 */
	[[nodiscard]] constexpr uint8_t GetDescriptorTypeBit(Xenon::Backend::DescriptorType type) noexcept
	{
		return static_cast<uint8_t>(1 << Xenon::EnumToInt(type));
	}
}

namespace Xenon
{
	namespace Backend
	{
		DX12RayTracingPipeline::DX12RayTracingPipeline(DX12Device* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, const RayTracingPipelineSpecification& specification)
			: RayTracingPipeline(pDevice, std::move(pCacheHandler), specification)
			, DX12DescriptorHeapManager(pDevice)
		{
			OPTICK_EVENT();

			auto rayTracingPipeline = CD3DX12_STATE_OBJECT_DESC(D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE);

			// Defines the maximum sizes in bytes for the ray rayPayload and attribute structure.
			auto shaderConfig = rayTracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
			shaderConfig->Config(specification.m_MaxPayloadSize, specification.m_MaxAttributeSize);

			// Setup shader groups.
			uint32_t index = 0;
			std::vector<std::wstring> groupNames;

			std::unordered_map<DescriptorType, std::unordered_map<uint32_t, DescriptorBindingInfo>> bindingMap;
			std::unordered_map<uint8_t, std::vector<CD3DX12_DESCRIPTOR_RANGE1>> globalRangeMap;

			for (const auto& group : specification.m_ShaderGroups)
			{
				std::vector<std::wstring> names;
				std::unordered_map<uint8_t, std::vector<CD3DX12_DESCRIPTOR_RANGE1>> rangeMap;

				auto pHitGroup = rayTracingPipeline.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
				pHitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

				const auto& groupName = groupNames.emplace_back(fmt::format(L"group{}", index));
				pHitGroup->SetHitGroupExport(groupName.c_str());

				if (group.m_RayGenShader.getDXIL().isValid())
				{
					const auto& newName = names.emplace_back(fmt::format(L"rayGenMain_group{}", index));
					SetupShaderData(group.m_RayGenShader, bindingMap, rangeMap, ShaderType::RayGen);
					createDXILLibrary(rayTracingPipeline, CD3DX12_SHADER_BYTECODE(group.m_RayGenShader.getDXIL().getBinaryData(), group.m_RayGenShader.getDXIL().getBinarySizeInBytes()), newName);
				}

				if (group.m_IntersectionShader.getDXIL().isValid())
				{
					const auto& newName = names.emplace_back(fmt::format(L"intersectionMain_group{}", index));
					SetupShaderData(group.m_IntersectionShader, bindingMap, rangeMap, ShaderType::Intersection);
					createDXILLibrary(rayTracingPipeline, CD3DX12_SHADER_BYTECODE(group.m_IntersectionShader.getDXIL().getBinaryData(), group.m_IntersectionShader.getDXIL().getBinarySizeInBytes()), newName);

					pHitGroup->SetIntersectionShaderImport(newName.c_str());
				}

				if (group.m_AnyHitShader.getDXIL().isValid())
				{
					const auto& newName = names.emplace_back(fmt::format(L"anyHitMain_group{}", index));
					SetupShaderData(group.m_AnyHitShader, bindingMap, rangeMap, ShaderType::AnyHit);
					createDXILLibrary(rayTracingPipeline, CD3DX12_SHADER_BYTECODE(group.m_AnyHitShader.getDXIL().getBinaryData(), group.m_AnyHitShader.getDXIL().getBinarySizeInBytes()), newName);

					pHitGroup->SetAnyHitShaderImport(newName.c_str());
				}

				if (group.m_ClosestHitShader.getDXIL().isValid())
				{
					const auto& newName = names.emplace_back(fmt::format(L"closestHitMain_group{}", index));
					SetupShaderData(group.m_ClosestHitShader, bindingMap, rangeMap, ShaderType::ClosestHit);
					createDXILLibrary(rayTracingPipeline, CD3DX12_SHADER_BYTECODE(group.m_ClosestHitShader.getDXIL().getBinaryData(), group.m_ClosestHitShader.getDXIL().getBinarySizeInBytes()), newName);

					pHitGroup->SetClosestHitShaderImport(newName.c_str());
				}

				if (group.m_MissShader.getDXIL().isValid())
				{
					const auto& newName = names.emplace_back(fmt::format(L"missMain_group{}", index));
					SetupShaderData(group.m_MissShader, bindingMap, rangeMap, ShaderType::Miss);
					createDXILLibrary(rayTracingPipeline, CD3DX12_SHADER_BYTECODE(group.m_MissShader.getDXIL().getBinaryData(), group.m_MissShader.getDXIL().getBinarySizeInBytes()), newName);
				}

				if (group.m_CallableShader.getDXIL().isValid())
				{
					const auto& newName = names.emplace_back(fmt::format(L"callableMain_group{}", index));
					SetupShaderData(group.m_CallableShader, bindingMap, rangeMap, ShaderType::Callable);
					createDXILLibrary(rayTracingPipeline, CD3DX12_SHADER_BYTECODE(group.m_CallableShader.getDXIL().getBinaryData(), group.m_CallableShader.getDXIL().getBinarySizeInBytes()), newName);
				}

				// Sort the ranges to the correct binding order.
				auto sortedRanges = std::vector<std::pair<uint8_t, std::vector<CD3DX12_DESCRIPTOR_RANGE1>>>(rangeMap.begin(), rangeMap.end());
				std::ranges::sort(sortedRanges, [](const auto& lhs, const auto& rhs) { return lhs.first < rhs.first; });

				// Create the global root signature.
				auto pRootSignature = createLocalRootSignature(std::move(sortedRanges));

				auto pLocalRootSignature = rayTracingPipeline.CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
				pLocalRootSignature->SetRootSignature(pRootSignature);

				std::vector<LPCTSTR> rawNames;
				rawNames.reserve(names.size());

				for (const auto& name : names)
					rawNames.emplace_back(name.c_str());

				// Setup the association.
				auto pRootSignatureAssociation = rayTracingPipeline.CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
				pRootSignatureAssociation->SetSubobjectToAssociate(*pLocalRootSignature);
				pRootSignatureAssociation->AddExports(rawNames.data(), static_cast<UINT>(rawNames.size()));

				// Insert the local group.
				// if (globalRangeMap.empty())
				// {
				// 	globalRangeMap = rangeMap;
				// }
				// else
				// {
				// 	for (const auto& [set, bindings] : rangeMap)
				// 	{
				// 		auto& globalRangeSlot = globalRangeMap[set];
				// 
				// 		for (const auto& range : bindings)
				// 		{
				// 			bool isContained = false;
				// 			for (const auto& candidate : globalRangeSlot)
				// 			{
				// 				if (candidate.BaseShaderRegister == range.BaseShaderRegister && candidate.RegisterSpace == range.RegisterSpace)
				// 				{
				// 					isContained = true;
				// 					break;
				// 				}
				// 			}
				// 
				// 			if (!isContained)
				// 				globalRangeSlot.emplace_back(range);
				// 		}
				// 	}
				// }

				index++;
			}

			// Setup the descriptor heap manager.
			setupDescriptorHeapManager(std::move(bindingMap));

			// Sort the ranges to the correct binding order.
			auto sortedRanges = std::vector<std::pair<uint8_t, std::vector<CD3DX12_DESCRIPTOR_RANGE1>>>(globalRangeMap.begin(), globalRangeMap.end());
			std::ranges::sort(sortedRanges, [](const auto& lhs, const auto& rhs) { return lhs.first < rhs.first; });

			// Create the global root signature.
			createGlobalRootSignature(std::move(sortedRanges));

			// Set the global root signature.
			auto pGlobalRootSignature = rayTracingPipeline.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
			pGlobalRootSignature->SetRootSignature(m_GlobalRootSignature.Get());

			// Setup the maximum ray recursion.
			auto pPipelineConfig = rayTracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
			pPipelineConfig->Config(specification.m_MaxRayRecursionDepth);

			// Create the state object.
			XENON_DX12_ASSERT(pDevice->getDevice()->CreateStateObject(rayTracingPipeline, IID_PPV_ARGS(&m_PipelineState)), "Failed to crate the ray tracing state object!");
			XENON_DX12_ASSERT(m_PipelineState.As(&m_PipelineStateProperties), "Failed to get the pipeline state object as state object properties!");
		}

		std::unique_ptr<Xenon::Backend::Descriptor> DX12RayTracingPipeline::createDescriptor(DescriptorType type)
		{
			OPTICK_EVENT();

			return std::make_unique<DX12Descriptor>(m_pDevice, getBindingInfo(type), type, this);
		}

		std::unique_ptr<ShaderBindingTable> DX12RayTracingPipeline::createShaderBindingTable(const std::vector<BindingGroup>& bindingGroups)
		{
			OPTICK_EVENT();

			return std::make_unique<DX12ShaderBindingTable>(m_pDevice, this, bindingGroups);
		}

		void* DX12RayTracingPipeline::getShaderID(ShaderType type, uint32_t group) const
		{
			switch (type)
			{
			case ShaderType::RayGen:
			{
				const auto groupName = fmt::format(L"rayGenMain_group{}", group);
				return m_PipelineStateProperties->GetShaderIdentifier(groupName.c_str());
				break;
			}

			case ShaderType::Intersection:
			case ShaderType::AnyHit:
			case ShaderType::ClosestHit:
			{
				const auto groupName = fmt::format(L"group{}", group);
				return m_PipelineStateProperties->GetShaderIdentifier(groupName.c_str());
				break;
			}

			case ShaderType::Miss:
			{
				const auto groupName = fmt::format(L"missMain_group{}", group);
				return m_PipelineStateProperties->GetShaderIdentifier(groupName.c_str());
				break;
			}

			case ShaderType::Callable:
			{
				const auto groupName = fmt::format(L"callableMain_group{}", group);
				return m_PipelineStateProperties->GetShaderIdentifier(groupName.c_str());
				break;
			}

			default:
				XENON_LOG_ERROR("Invalid shader type provided to the shader builder! The only supported shader types are RayGen, Intersection, AnyHit, ClosestHit, Miss and Callable.");
				break;
			}

			return nullptr;
		}

		void DX12RayTracingPipeline::createDXILLibrary(CD3DX12_STATE_OBJECT_DESC& stateObject, D3D12_SHADER_BYTECODE shader, const std::wstring_view& newExport) const
		{
			OPTICK_EVENT();

			auto pLibrary = stateObject.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
			pLibrary->SetDXILLibrary(&shader);
			pLibrary->DefineExport(newExport.data(), L"main");
		}

		ID3D12RootSignature* DX12RayTracingPipeline::createLocalRootSignature(std::vector<std::pair<uint8_t, std::vector<CD3DX12_DESCRIPTOR_RANGE1>>>&& rangePairs)
		{
			OPTICK_EVENT();

			std::vector<CD3DX12_ROOT_PARAMETER1> rootParameters;
			for (const auto& [set, ranges] : rangePairs)
				rootParameters.emplace_back().InitAsDescriptorTable(static_cast<UINT>(ranges.size()), ranges.data());

			D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

			if (FAILED(m_pDevice->getDevice()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
				featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;

			// Create the root signature.
			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
			rootSignatureDesc.Init_1_1(static_cast<UINT>(rootParameters.size()), rootParameters.data(), 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE);

			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;
			XENON_DX12_ASSERT(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error), "Failed to serialize the version-ed root signature!");
			XENON_DX12_ASSERT_BLOB(error);

			auto& rootSignature = m_LocalRootSignatures.emplace_back();
			XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)), "Failed to create the local root signature!");
			XENON_DX12_NAME_OBJECT(rootSignature, "Local Ray Tracing Root Signature");

			return rootSignature.Get();
		}

		void DX12RayTracingPipeline::createGlobalRootSignature(std::vector<std::pair<uint8_t, std::vector<CD3DX12_DESCRIPTOR_RANGE1>>>&& rangePairs)
		{
			OPTICK_EVENT();

			std::vector<CD3DX12_ROOT_PARAMETER1> rootParameters;
			for (const auto& [set, ranges] : rangePairs)
				rootParameters.emplace_back().InitAsDescriptorTable(static_cast<UINT>(ranges.size()), ranges.data());

			D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

			if (FAILED(m_pDevice->getDevice()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
				featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;

			// Create the root signature.
			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
			rootSignatureDesc.Init_1_1(static_cast<UINT>(rootParameters.size()), rootParameters.data());

			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;
			XENON_DX12_ASSERT(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error), "Failed to serialize the version-ed root signature!");
			XENON_DX12_ASSERT_BLOB(error);

			XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_GlobalRootSignature)), "Failed to create the global root signature!");
			XENON_DX12_NAME_OBJECT(m_GlobalRootSignature, "Global Ray Tracing Root Signature");
		}
	}
}