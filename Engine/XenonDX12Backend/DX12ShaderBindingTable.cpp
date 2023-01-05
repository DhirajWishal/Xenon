// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12ShaderBindingTable.hpp"
#include "DX12Macros.hpp"

#include "DX12Image.hpp"
#include "DX12Buffer.hpp"

namespace /* anonymous */
{
	/**
	 * Get the entry size from a data variant.
	 *
	 * @param entry The entry to get the size of.
	 * @return The size.
	 */
	[[nodiscard]] uint64_t GetEntrySize(const Xenon::Backend::BindingGroup::DataVariant& entry) noexcept
	{
		uint64_t entrySize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;

		switch (entry.index())
		{
		case 0:
			entrySize += sizeof(std::get<0>(entry));
			break;

		case 1:
			entrySize += sizeof(std::get<1>(entry));
			break;

		case 2:
			entrySize += std::get<2>(entry).second;
			break;

		default:
			break;
		}

		return XENON_ALIGNED_SIZE_2(entrySize, D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT);
	}

	/**
	 * Copy data to a destination pointer and increment the destination pointer.
	 * This will not take the alignment into consideration when incrementing the pointer.
	 *
	 * @param pSource The source data pointer.
	 * @param pDestination The destination data pointer. This will be incremented by size.
	 * @param size The number of bytes to copy.
	 */
	void CopyIncrementWithoutAlignment(const std::byte* pSource, std::byte*& pDestination, uint64_t size)
	{
		// Copy only if we have valid data.
		if (pSource)
			std::copy_n(pSource, size, pDestination);

		pDestination += size;
	}

	/**
	 * Copy data to a destination pointer and increment the destination pointer.
	 *
	 * @param pSource The source data pointer.
	 * @param pDestination The destination data pointer. This will be incremented by size.
	 * @param size The number of bytes to copy.
	 */
	void CopyIncrement(const std::byte* pSource, std::byte*& pDestination, uint64_t size)
	{
		// Copy only if we have valid data.
		if (pSource)
			std::copy_n(pSource, size, pDestination);

		pDestination += XENON_ALIGNED_SIZE_2(size, D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT);
	}

	/**
	 * Copy an entry to the destination pointer.
	 * This will also increment the pointer.
	 *
	 * @param entry The entry to copy.
	 * @param pDestination The destination pointer.
	 */
	void CopyEntry(const Xenon::Backend::BindingGroup::DataVariant& entry, std::byte*& pDestination)
	{
		switch (entry.index())
		{
		case 0:
		{
			auto address = std::get<0>(entry)->as<Xenon::Backend::DX12Buffer>()->getResource()->GetGPUVirtualAddress();
			CopyIncrement(Xenon::ToBytes(&address), pDestination, sizeof(address));
			break;
		}

		case 1:
		{
			auto address = std::get<1>(entry)->as<Xenon::Backend::DX12Image>()->getResource()->GetGPUVirtualAddress();
			CopyIncrement(Xenon::ToBytes(&address), pDestination, sizeof(address));
		}
		break;

		case 2:
			CopyIncrement(std::get<2>(entry).first, pDestination, std::get<2>(entry).second);
			break;

		default:
			break;
		}
	}
}

namespace Xenon
{
	namespace Backend
	{
		DX12ShaderBindingTable::DX12ShaderBindingTable(DX12Device* pDevice, DX12RayTracingPipeline* pPipeline, const std::vector<BindingGroup>& bindingGroups)
			: ShaderBindingTable(pDevice, pPipeline, bindingGroups)
			, DX12DeviceBoundObject(pDevice)
		{
			// Validate the binding group count.
			if (pPipeline->getSpecification().m_ShaderGroups.size() > bindingGroups.size())
			{
				XENON_LOG_ERROR("Failed to create the shader binding table! The binding group count should not be grater than the pipeline's shader group count.");
				return;
			}

			// Get the allocation sizes.
			for (const auto& group : bindingGroups)
			{
				for (const auto& [shader, entry] : group.m_Entries)
				{
					switch (shader)
					{
					case ShaderType::RayGen:
						m_RayGenSize += GetEntrySize(entry);
						break;

					case ShaderType::Intersection:
					case ShaderType::AnyHit:
					case ShaderType::ClosestHit:
						m_RayHitSize += GetEntrySize(entry);
						break;

					case ShaderType::Miss:
						m_RayMissSize += GetEntrySize(entry);
						break;

					case ShaderType::Callable:
						m_CallableSize += GetEntrySize(entry);
						break;

					default:
						XENON_LOG_ERROR("Invalid shader type provided to the shader builder! The only supported shader types are RayGen, Intersection, AnyHit, ClosestHit, Miss and Callable.");
						break;
					}
				}
			}

			// Create the buffer.
			const CD3DX12_RESOURCE_DESC resourceDescriptor = CD3DX12_RESOURCE_DESC::Buffer(m_RayGenSize + m_RayMissSize + m_RayHitSize + m_CallableSize);

			D3D12MA::ALLOCATION_DESC allocationDesc = {};
			allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

			XENON_DX12_ASSERT(pDevice->getAllocator()->CreateResource(
				&allocationDesc,
				&resourceDescriptor,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				&m_pAllocation,
				IID_NULL,
				nullptr), "Failed to create the shader binding table buffer!");

			XENON_DX12_NAME_OBJECT(m_pAllocation, "Shader Binding Table");

			// Get the buffer memory.
			auto pRayGenMemory = map();
			auto pMissMemory = pRayGenMemory + m_RayGenSize;
			auto pHitGroupMemory = pMissMemory + m_RayMissSize;
			auto pCallableMemory = pHitGroupMemory + m_RayHitSize;

			// Copy the data.
			uint32_t index = 0;
			for (const auto& group : bindingGroups)
			{
				for (const auto& [shaderType, entry] : group.m_Entries)
				{
					const void* pShaderID = pPipeline->getShaderID(shaderType, index);

					switch (shaderType)
					{
					case ShaderType::RayGen:
						CopyIncrementWithoutAlignment(ToBytes(pShaderID), pRayGenMemory, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
						CopyEntry(entry, pRayGenMemory);
						break;

					case ShaderType::Intersection:
					case ShaderType::AnyHit:
					case ShaderType::ClosestHit:
						CopyIncrementWithoutAlignment(ToBytes(pShaderID), pHitGroupMemory, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
						CopyEntry(entry, pHitGroupMemory);
						break;

					case ShaderType::Miss:
						CopyIncrementWithoutAlignment(ToBytes(pShaderID), pMissMemory, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
						CopyEntry(entry, pMissMemory);
						break;

					case ShaderType::Callable:
						CopyIncrementWithoutAlignment(ToBytes(pShaderID), pCallableMemory, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
						CopyEntry(entry, pCallableMemory);
						break;

					default:
						break;
					}
				}

				index++;
			}

			// Finally unmap the memory.
			unmap();
		}

		DX12ShaderBindingTable::~DX12ShaderBindingTable()
		{
			m_pAllocation->Release();
		}

		std::byte* DX12ShaderBindingTable::map()
		{
			std::byte* pMemory = nullptr;
			m_pAllocation->GetResource()->Map(0, nullptr, std::bit_cast<void**>(&pMemory));

			return pMemory;
		}

		void DX12ShaderBindingTable::unmap()
		{
			m_pAllocation->GetResource()->Unmap(0, nullptr);
		}
	}
}