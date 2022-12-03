// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12DescriptorHeapManager.hpp"
#include "DX12Macros.hpp"

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
}

namespace Xenon
{
	namespace Backend
	{
		DX12DescriptorHeapManager::DX12DescriptorHeapManager(DX12Device* pDevice)
			: DX12DeviceBoundObject(pDevice)
		{
		}

		void DX12DescriptorHeapManager::setupDescriptorHeapManager(std::unordered_map<DescriptorType, std::vector<DescriptorBindingInfo>>&& bindingMap)
		{
			m_BindingMap = std::move(bindingMap);

			// Sort the bindings.
			auto sortedBindings = std::vector<std::pair<DescriptorType, std::vector<DescriptorBindingInfo>>>(m_BindingMap.begin(), m_BindingMap.end());
			std::ranges::sort(sortedBindings, [](const auto& lhs, const auto& rhs) { return EnumToInt(lhs.first) < EnumToInt(rhs.first); });

			for (const auto& [type, bindingInfos] : sortedBindings)
			{
				for (const auto& info : bindingInfos)
				{
					m_CbvSrvUavCount++;
					m_GroupSizes[type].first++;
					m_SamplerIndex.emplace_back(m_SamplerCount);

					if (info.m_Type == ResourceType::Sampler || info.m_Type == ResourceType::CombinedImageSampler)
					{
						m_SamplerCount++;
						m_GroupSizes[type].second++;
					}

					m_Ranges.emplace_back().Init(GetDescriptorRangeType(info.m_Type), 1, 0);
				}
			}

			// Setup and increment the heaps.
			incrementHeaps();
		}

		Xenon::Backend::DX12PipelineDescriptorHeapStorage& DX12DescriptorHeapManager::getDescriptorHeapStorage()
		{
			OPTICK_EVENT();

			if (m_IsUpdated)
			{
				D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
				heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

				if (m_CbvSrvUavCount > 0)
				{
					heapDesc.NumDescriptors = m_CbvSrvUavDescriptorCount;
					heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
					XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_ShaderVisibleCbvSrvUavDescriptorHeap)), "Failed to create the CBV SRV UAV descriptor heap!");

					// Copy the old data to the new heap.
					if (m_CbvSrvUavDescriptorHeap)
					{
						m_pDevice->getDevice()->CopyDescriptorsSimple(
							m_CbvSrvUavDescriptorCount,
							m_ShaderVisibleCbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
							m_CbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
							D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
						);

						m_ShaderVisibleHeaps[0] = m_ShaderVisibleCbvSrvUavDescriptorHeap.Get();
					}
				}

				if (m_SamplerCount > 0)
				{
					heapDesc.NumDescriptors = m_SamplerDescriptorCount;
					heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
					XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_ShaderVisibleSamplerDescriptorHeap)), "Failed to create the sampler descriptor heap!");

					// Copy the old data to the new heap.
					if (m_SamplerDescriptorHeap)
					{
						m_pDevice->getDevice()->CopyDescriptorsSimple(
							m_SamplerDescriptorCount,
							m_ShaderVisibleSamplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
							m_SamplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
							D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER
						);
					}

					m_ShaderVisibleHeaps[1] = m_ShaderVisibleSamplerDescriptorHeap.Get();
				}
			}

			m_IsUpdated = false;
			return m_ShaderVisibleHeaps;
		}

		std::pair<UINT, UINT> DX12DescriptorHeapManager::setupDescriptor(DescriptorType type)
		{
			OPTICK_EVENT();

			auto newPair = std::make_pair(m_CbvSrvUavDescriptorCount, m_SamplerDescriptorCount);

			const auto& [bufferCount, samplerCount] = m_GroupSizes[type];
			m_CbvSrvUavDescriptorCount += bufferCount;
			m_SamplerDescriptorCount += samplerCount;
			incrementHeaps();

			return newPair;
		}

		UINT DX12DescriptorHeapManager::getNextSize(UINT newSize, UINT oldSize) const
		{
			OPTICK_EVENT();

			const auto nextSize = oldSize + oldSize / 2;
			if (nextSize < newSize)
				return newSize;

			return nextSize;
		}

		void DX12DescriptorHeapManager::incrementHeaps()
		{
			OPTICK_EVENT();

			D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
			heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

			if (m_CbvSrvUavCount > 0 && m_AllocatedCbvSrvUavDescriptorCount <= m_CbvSrvUavDescriptorCount)
			{
				m_AllocatedCbvSrvUavDescriptorCount = getNextSize(m_CbvSrvUavDescriptorCount + m_CbvSrvUavCount, m_CbvSrvUavDescriptorCount);
				heapDesc.NumDescriptors = m_AllocatedCbvSrvUavDescriptorCount;
				heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

				ComPtr<ID3D12DescriptorHeap> newHeap;
				XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&newHeap)), "Failed to create the CBV SRV UAV descriptor heap!");

				// Copy the old data to the new heap.
				if (m_CbvSrvUavDescriptorHeap)
				{
					m_pDevice->getDevice()->CopyDescriptorsSimple(
						m_CbvSrvUavDescriptorCount,
						newHeap->GetCPUDescriptorHandleForHeapStart(),
						m_CbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
						D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
					);
				}

				m_CbvSrvUavDescriptorHeap = std::move(newHeap);
			}

			if (m_SamplerCount && m_AllocatedSamplerDescriptorCount <= m_SamplerDescriptorCount)
			{
				m_AllocatedSamplerDescriptorCount = getNextSize(m_SamplerDescriptorCount + m_SamplerCount, m_SamplerDescriptorCount);
				heapDesc.NumDescriptors = m_AllocatedSamplerDescriptorCount;
				heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;

				ComPtr<ID3D12DescriptorHeap> newHeap;
				XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&newHeap)), "Failed to create the sampler descriptor heap!");

				// Copy the old data to the new heap.
				if (m_SamplerDescriptorHeap)
				{
					m_pDevice->getDevice()->CopyDescriptorsSimple(
						m_SamplerDescriptorCount,
						newHeap->GetCPUDescriptorHandleForHeapStart(),
						m_SamplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
						D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER
					);
				}

				m_SamplerDescriptorHeap = std::move(newHeap);
			}
		}
	}
}