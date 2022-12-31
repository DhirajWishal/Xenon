// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12RayTracingPipeline.hpp"
#include "DX12Macros.hpp"
#include "DX12Descriptor.hpp"

#include <spdlog/fmt/xchar.h>
#include <optick.h>

namespace /* anonymous */
{
	/**
	 * Get the descriptor range type.
	 *
	 * @param resource The Xenon resource type.
	 * @return The D3D12 descriptor range type.
	 */
	[[nodiscard]] constexpr D3D12_DESCRIPTOR_RANGE_TYPE GetDescriptorRangeType(Xenon::Backend::ResourceType resource) noexcept
	{
		switch (resource)
		{
		case Xenon::Backend::ResourceType::Sampler:
		case Xenon::Backend::ResourceType::CombinedImageSampler:
			return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;

		case Xenon::Backend::ResourceType::SampledImage:
			return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

		case Xenon::Backend::ResourceType::StorageImage:
			return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;

		case Xenon::Backend::ResourceType::UniformTexelBuffer:
			return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

		case Xenon::Backend::ResourceType::StorageTexelBuffer:
			return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;

		case Xenon::Backend::ResourceType::UniformBuffer:
			return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;

		case Xenon::Backend::ResourceType::StorageBuffer:
			return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;

		case Xenon::Backend::ResourceType::DynamicUniformBuffer:
			return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;

		case Xenon::Backend::ResourceType::DynamicStorageBuffer:
			return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;

		case Xenon::Backend::ResourceType::InputAttachment:
			return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

		case Xenon::Backend::ResourceType::AccelerationStructure:
			return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

		default:
			XENON_LOG_ERROR("Invalid resource type! Defaulting to SRV.");
			return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		}
	}

	/**
	 * Setup all the shader-specific data.
	 *
	 * @param shader The shader to get the data from.
	 * @param rangeMap The descriptor range map.
	 * @param type The shader's type.
	 */
	void SetupShaderData(
		const Xenon::Backend::Shader& shader,
		std::unordered_map<Xenon::Backend::DescriptorType, std::vector<Xenon::Backend::DescriptorBindingInfo>>& bindingMap,
		std::unordered_map<uint32_t, std::unordered_map<uint32_t, size_t>>& indexToBindingMap,
		std::unordered_map<uint8_t, std::vector<CD3DX12_DESCRIPTOR_RANGE1>>& rangeMap,
		Xenon::Backend::ShaderType type)
	{
		// Setup resources.
		for (const auto& resource : shader.getResources())
		{
			// Fill up the binding info structure.
			auto& bindings = bindingMap[static_cast<Xenon::Backend::DescriptorType>(Xenon::EnumToInt(resource.m_Set))];
			auto& indexToBinding = indexToBindingMap[Xenon::EnumToInt(resource.m_Set)];

			if (indexToBinding.contains(resource.m_Binding))
			{
				bindings[indexToBinding[resource.m_Binding]].m_ApplicableShaders |= type;
			}
			else
			{
				indexToBinding[resource.m_Binding] = bindings.size();
				auto& binding = bindings.emplace_back();
				binding.m_Type = resource.m_Type;
				binding.m_ApplicableShaders = type;
			}

			// Setup the rest.
			const auto rangeType = GetDescriptorRangeType(resource.m_Type);

			// If it's a sampler, we need one for the texture (SRV) and another as the sampler.
			if (rangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER)
			{
				const auto setIndex = static_cast<uint8_t>(Xenon::EnumToInt(resource.m_Set) * 2);
				rangeMap[setIndex + 0].emplace_back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, resource.m_Binding, Xenon::EnumToInt(resource.m_Set));	// Set the texture buffer (SRV).
				rangeMap[setIndex + 1].emplace_back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, resource.m_Binding, Xenon::EnumToInt(resource.m_Set));	// Set the texture sampler.
			}

			// Else just one entry for the buffer.
			else
			{
				rangeMap[static_cast<uint8_t>(Xenon::EnumToInt(resource.m_Set) * 2)].emplace_back().Init(rangeType, 1, resource.m_Binding, Xenon::EnumToInt(resource.m_Set));
			}
		}
	}
}

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
			std::vector<std::wstring> names;
			std::vector<std::wstring> groupNames;
			std::vector<std::wstring> importNames;

			std::unordered_map<DescriptorType, std::vector<DescriptorBindingInfo>> bindingMap;
			std::unordered_map<uint32_t, std::unordered_map<uint32_t, size_t>> indexToBindingMap;
			std::unordered_map<uint8_t, std::vector<CD3DX12_DESCRIPTOR_RANGE1>> rangeMap;

			for (const auto& group : shaderGroups)
			{
				auto pHitGroup = rayTracingPipeline.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
				pHitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

				const auto& groupName = groupNames.emplace_back(fmt::format(L"group{}", index));
				pHitGroup->SetHitGroupExport(groupName.c_str());

				if (group.m_RayGenShader.getDXIL().isValid())
				{
					const auto& newName = names.emplace_back(fmt::format(L"rayGenMain_group{}", index));
					SetupShaderData(group.m_RayGenShader, bindingMap, indexToBindingMap, rangeMap, ShaderType::RayGen);
					createDXILLibrary(rayTracingPipeline, CD3DX12_SHADER_BYTECODE(group.m_RayGenShader.getDXIL().getBinaryData(), group.m_RayGenShader.getDXIL().getBinarySizeInBytes()), newName);
				}

				if (group.m_IntersectionShader.getDXIL().isValid())
				{
					const auto& newName = names.emplace_back(fmt::format(L"intersectionMain_group{}", index));
					SetupShaderData(group.m_IntersectionShader, bindingMap, indexToBindingMap, rangeMap, ShaderType::Intersection);
					createDXILLibrary(rayTracingPipeline, CD3DX12_SHADER_BYTECODE(group.m_IntersectionShader.getDXIL().getBinaryData(), group.m_IntersectionShader.getDXIL().getBinarySizeInBytes()), newName);

					const auto& wNewName = importNames.emplace_back(fmt::format(L"rayGenMain_group{}", index));
					pHitGroup->SetIntersectionShaderImport(wNewName.c_str());
				}

				if (group.m_AnyHitShader.getDXIL().isValid())
				{
					const auto& newName = names.emplace_back(fmt::format(L"anyHitMain_group{}", index));
					SetupShaderData(group.m_AnyHitShader, bindingMap, indexToBindingMap, rangeMap, ShaderType::AnyHit);
					createDXILLibrary(rayTracingPipeline, CD3DX12_SHADER_BYTECODE(group.m_AnyHitShader.getDXIL().getBinaryData(), group.m_AnyHitShader.getDXIL().getBinarySizeInBytes()), newName);

					const auto& wNewName = importNames.emplace_back(fmt::format(L"rayGenMain_group{}", index));
					pHitGroup->SetAnyHitShaderImport(wNewName.c_str());
				}

				if (group.m_ClosestHitShader.getDXIL().isValid())
				{
					const auto& newName = names.emplace_back(fmt::format(L"closestHitMain_group{}", index));
					SetupShaderData(group.m_ClosestHitShader, bindingMap, indexToBindingMap, rangeMap, ShaderType::ClosestHit);
					createDXILLibrary(rayTracingPipeline, CD3DX12_SHADER_BYTECODE(group.m_ClosestHitShader.getDXIL().getBinaryData(), group.m_ClosestHitShader.getDXIL().getBinarySizeInBytes()), newName);

					const auto& wNewName = importNames.emplace_back(fmt::format(L"rayGenMain_group{}", index));
					pHitGroup->SetClosestHitShaderImport(wNewName.c_str());
				}

				if (group.m_MissShader.getDXIL().isValid())
				{
					const auto& newName = names.emplace_back(fmt::format(L"missMain_group{}", index));
					SetupShaderData(group.m_MissShader, bindingMap, indexToBindingMap, rangeMap, ShaderType::Miss);
					createDXILLibrary(rayTracingPipeline, CD3DX12_SHADER_BYTECODE(group.m_MissShader.getDXIL().getBinaryData(), group.m_MissShader.getDXIL().getBinarySizeInBytes()), newName);
				}

				if (group.m_CallableShader.getDXIL().isValid())
				{
					const auto& newName = names.emplace_back(fmt::format(L"callableMain_group{}", index));
					SetupShaderData(group.m_CallableShader, bindingMap, indexToBindingMap, rangeMap, ShaderType::Callable);
					createDXILLibrary(rayTracingPipeline, CD3DX12_SHADER_BYTECODE(group.m_CallableShader.getDXIL().getBinaryData(), group.m_CallableShader.getDXIL().getBinarySizeInBytes()), newName);
				}

				index++;
			}

			// Sort the ranges to the correct binding order.
			auto sortedRanges = std::vector<std::pair<uint8_t, std::vector<CD3DX12_DESCRIPTOR_RANGE1>>>(rangeMap.begin(), rangeMap.end());
			std::ranges::sort(sortedRanges, [](const auto& lhs, const auto& rhs) { return lhs.first < rhs.first; });

			// Setup the descriptor heap manager.
			setupDescriptorHeapManager(std::move(bindingMap));

			// Create the root signature.
			createRootSignature(std::move(sortedRanges));

			// Set the global root signature.
			auto globalRootSignature = rayTracingPipeline.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
			globalRootSignature->SetRootSignature(m_RootSignature.Get());

			// Setup the maximum ray recursion.
			auto pipelineConfig = rayTracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
			pipelineConfig->Config(maxRayRecursion);

			// Create the state object.
			XENON_DX12_ASSERT(pDevice->getDevice()->CreateStateObject(rayTracingPipeline, IID_PPV_ARGS(&m_PipelineState)), "Failed to crate the ray tracing state object!");
		}

		std::unique_ptr<Xenon::Backend::Descriptor> DX12RayTracingPipeline::createDescriptor(DescriptorType type)
		{
			OPTICK_EVENT();

			return std::make_unique<DX12Descriptor>(m_pDevice, getBindingInfo(type), type, this);
		}

		void DX12RayTracingPipeline::createDXILLibrary(CD3DX12_STATE_OBJECT_DESC& stateObject, D3D12_SHADER_BYTECODE shader, const std::wstring_view& newExport) const
		{
			OPTICK_EVENT();

			auto pLibrary = stateObject.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
			pLibrary->SetDXILLibrary(&shader);
			pLibrary->DefineExport(L"main", newExport.data());
		}

		void DX12RayTracingPipeline::createRootSignature(std::vector<std::pair<uint8_t, std::vector<CD3DX12_DESCRIPTOR_RANGE1>>>&& rangePairs)
		{
			OPTICK_EVENT();

			std::vector<CD3DX12_ROOT_PARAMETER1> rootParameters;
			for (const auto& [set, ranges] : rangePairs)
				rootParameters.emplace_back().InitAsDescriptorTable(static_cast<UINT>(ranges.size()), ranges.data(), D3D12_SHADER_VISIBILITY_ALL);

			D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

			if (FAILED(m_pDevice->getDevice()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
				featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;

			// Create the root signature.
			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
			rootSignatureDesc.Init_1_1(static_cast<UINT>(rootParameters.size()), rootParameters.data(), 0, nullptr);

			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;
			XENON_DX12_ASSERT(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error), "Failed to serialize the version-ed root signature!");
			XENON_DX12_ASSERT_BLOB(error);

			XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)), "Failed to create the root signature!");
			XENON_DX12_NAME_OBJECT(m_RootSignature, "Ray Tracing Root Signature");
		}
	}
}