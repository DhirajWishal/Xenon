// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12ComputePipeline.hpp"
#include "DX12Macros.hpp"
#include "DX12Descriptor.hpp"

#include <spirv_hlsl.hpp>
#include <optick.h>

// This magic number is used by the rasterizing pipeline to uniquely identify it's pipeline caches.
constexpr auto g_MagicNumber = 0b0111100101110000101100010000110010100010001110011100010100011001;

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
	 * @param bindingInfos The descriptor binding infos.
	 * @param descriptorRanges The descriptor ranges.
	 */
	void SetupShaderData(
		const Xenon::Backend::Shader& shader,
		std::vector<Xenon::Backend::DescriptorBindingInfo>& bindingInfos,
		std::vector<CD3DX12_DESCRIPTOR_RANGE1>& descriptorRanges)
	{
		ComPtr<ID3D12ShaderReflection> pReflector;
		XENON_DX12_ASSERT(D3DReflect(shader.getDXIL().getBinaryData(), shader.getDXIL().getBinarySizeInBytes(), IID_PPV_ARGS(&pReflector)), "Failed to reflect on the shader!");

		D3D12_SHADER_DESC shaderDesc = {};
		XENON_DX12_ASSERT(pReflector->GetDesc(&shaderDesc), "Failed to get the reflection description!");

		// Setup resources.
		for (const auto& resource : shader.getSPIRV().getResources())
		{
			auto& binding = bindingInfos.emplace_back();
			binding.m_Type = resource.m_Type;
			binding.m_ApplicableShaders = Xenon::Backend::ShaderType::Compute;

			// Setup the rest.
			const auto rangeType = GetDescriptorRangeType(resource.m_Type);

			// If it's a sampler, we need one for the texture (SRV) and another as the sampler.
			if (rangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER)
			{
				descriptorRanges.emplace_back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, resource.m_Binding, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);	// Set the texture buffer (SRV).
				descriptorRanges.emplace_back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, resource.m_Binding);	// Set the texture sampler.
			}

			// Else just one entry for the buffer.
			else
			{
				descriptorRanges.emplace_back().Init(rangeType, 1, resource.m_Binding, rangeType == D3D12_DESCRIPTOR_RANGE_TYPE_UAV ? D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE : D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
			}
		}
	}
}

namespace Xenon
{
	namespace Backend
	{
		DX12ComputePipeline::DX12ComputePipeline(DX12Device* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, const Shader& computeShader)
			: ComputePipeline(pDevice, std::move(pCacheHandler), computeShader)
			, DX12DescriptorHeapManager(pDevice)
		{
			// Setup the shader information.
			std::vector<CD3DX12_DESCRIPTOR_RANGE1> descriptorRanges;
			SetupShaderData(computeShader, m_BindingInfos, descriptorRanges);

			// Generate the pipeline hash.
			m_PipelineHash = GenerateHash(ToBytes(computeShader.getDXIL().getBinary().data()), computeShader.getDXIL().getBinary().size() * sizeof(uint64_t));

			// Setup the descriptor heap manager.
			setupDescriptorHeapManager({ { DescriptorType::UserDefined, m_BindingInfos } });

			// Create the root signature.
			createRootSignature(std::move(descriptorRanges));

			// Create the pipeline state object.
			createPipelineStateObject();
		}

		std::unique_ptr<Xenon::Backend::Descriptor> DX12ComputePipeline::createDescriptor(DescriptorType type)
		{
			OPTICK_EVENT();

			return std::make_unique<DX12Descriptor>(m_pDevice, m_BindingInfos, DescriptorType::UserDefined, this);
		}

		void DX12ComputePipeline::createRootSignature(std::vector<CD3DX12_DESCRIPTOR_RANGE1>&& descriptorRanges)
		{
			CD3DX12_ROOT_PARAMETER1 rootParameter = {};
			rootParameter.InitAsDescriptorTable(static_cast<UINT>(descriptorRanges.size()), descriptorRanges.data(), D3D12_SHADER_VISIBILITY_ALL);

			D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

			if (FAILED(m_pDevice->getDevice()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
				featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;

			// Create the root signature.
			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.Init_1_1(1, &rootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;
			XENON_DX12_ASSERT(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error), "Failed to serialize the version-ed root signature!");
			XENON_DX12_ASSERT_BLOB(error);

			XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)), "Failed to create the root signature!");
			XENON_DX12_NAME_OBJECT(m_RootSignature, "Compute Root Signature");
		}

		std::vector<std::byte> DX12ComputePipeline::loadPipelineSateCache() const
		{
			OPTICK_EVENT();

			if (m_pCacheHandler)
				return m_pCacheHandler->load(m_PipelineHash ^ g_MagicNumber);

			XENON_LOG_INFORMATION("A pipeline cache handler is not attached to load the pipeline cache data.");
			return {};
		}

		void DX12ComputePipeline::storePipelineStateCache() const
		{
			OPTICK_EVENT();

			if (m_pCacheHandler)
			{
				ComPtr<ID3DBlob> cacheBlob;
				XENON_DX12_ASSERT(m_PipelineState->GetCachedBlob(&cacheBlob), "Failed to get the pipeline state object's cache!");

				m_pCacheHandler->store(m_PipelineHash ^ g_MagicNumber, std::vector<std::byte>(ToBytes(cacheBlob->GetBufferPointer()), ToBytes(cacheBlob->GetBufferPointer()) + cacheBlob->GetBufferSize()));
			}
			else
			{
				XENON_LOG_INFORMATION("A pipeline cache handler is not attached to store the pipeline cache data.");
			}
		}

		void DX12ComputePipeline::createPipelineStateObject()
		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC computeDesc = {};
			computeDesc.pRootSignature = m_RootSignature.Get();
			computeDesc.CS = CD3DX12_SHADER_BYTECODE(m_ComputeShader.getDXIL().getBinaryData(), m_ComputeShader.getDXIL().getBinarySizeInBytes());

			// Load the pipeline cache.
			const auto cache = loadPipelineSateCache();
			computeDesc.CachedPSO.pCachedBlob = cache.data();
			computeDesc.CachedPSO.CachedBlobSizeInBytes = cache.size();

			// Try and create the pipeline with the cache.
			// If it failed, try and create the pipeline without the cache.
			if (FAILED(m_pDevice->getDevice()->CreateComputePipelineState(&computeDesc, IID_PPV_ARGS(&m_PipelineState))))
			{
				computeDesc.CachedPSO.pCachedBlob = nullptr;
				computeDesc.CachedPSO.CachedBlobSizeInBytes = 0;

				XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateComputePipelineState(&computeDesc, IID_PPV_ARGS(&m_PipelineState)), "Failed to create the compute pipeline!");
				XENON_DX12_NAME_OBJECT(m_PipelineState, "Compute Pipeline State");
			}

			// Store the pipeline cache.
			storePipelineStateCache();
		}
	}
}