// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#include "VulkanShaderBindingTable.hpp"
#include "VulkanMacros.hpp"

#include "VulkanImage.hpp"
#include "VulkanBuffer.hpp"

#ifdef max
#undef max

#endif

namespace /* anonymous */
{
	/**
	 * Get the entry size from a data variant.
	 *
	 * @param entry The entry to get the size of.
	 * @param handleSize The size of the handle.
	 * @param entryAlignment The alignment of the whole entry.
	 * @return The size.
	 */
	[[nodiscard]] uint64_t GetEntrySize(const Xenon::Backend::BindingGroup::DataVariant& entry, uint32_t handleSize, uint64_t entryAlignment) noexcept
	{
		uint64_t entrySize = handleSize;

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

		return  XENON_ALIGNED_SIZE_2(entrySize, entryAlignment);
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

		pDestination += size;
	}

	/**
	 * Copy data to a destination pointer and increment the destination pointer.
	 *
	 * @param pSource The source data pointer.
	 * @param pDestination The destination data pointer. This will be incremented by size.
	 * @param size The number of bytes to copy.
	 * @param stride The stride of the entry.
	 */
	void CopyIncrement(const std::byte* pSource, std::byte*& pDestination, uint64_t size, uint64_t stride)
	{
		// Copy only if we have valid data.
		if (pSource)
			std::copy_n(pSource, size, pDestination);

		pDestination += stride;
	}

	/**
	 * Copy an entry to the destination pointer.
	 * This will also increment the pointer.
	 *
	 * @param entry The entry to copy.
	 * @param pDestination The destination pointer.
	 * @param stride The stride of the entry.
	 */
	void CopyEntry(const Xenon::Backend::BindingGroup::DataVariant& entry, std::byte*& pDestination, uint64_t stride)
	{
		switch (entry.index())
		{
		case 0:
		{
			auto address = std::get<0>(entry)->as<Xenon::Backend::VulkanBuffer>()->getDeviceAddress();
			CopyIncrement(Xenon::ToBytes(&address), pDestination, sizeof(address), stride);
			break;
		}

		case 1:
		{
			// auto address = std::get<1>(entry)->as<Xenon::Backend::VulkanImage>()->getde();
			// CopyIncrement(Xenon::ToBytes(&address), pDestination, sizeof(address), stride);
			break;
		}

		case 2:
			CopyIncrement(std::get<2>(entry).first, pDestination, std::get<2>(entry).second, stride);
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
		VulkanShaderBindingTable::VulkanShaderBindingTable(VulkanDevice* pDevice, VulkanRayTracingPipeline* pPipeline, const std::vector<BindingGroup>& bindingGroups)
			: ShaderBindingTable(pDevice, pPipeline, bindingGroups)
			, VulkanDeviceBoundObject(pDevice)
		{
			if (pPipeline->getSpecification().m_ShaderGroups.size() < bindingGroups.size())
			{
				XENON_LOG_ERROR("Failed to create the shader binding table! The binding group count should not be grater than the pipeline's shader group count.");
				return;
			}

			const auto handleSize = m_pDevice->getPhysicalDeviceRayTracingPipelineProperties().shaderGroupHandleSize;
			const auto entryAlignment = m_pDevice->getPhysicalDeviceRayTracingPipelineProperties().shaderGroupHandleAlignment;
			const auto maxStride = m_pDevice->getPhysicalDeviceRayTracingPipelineProperties().maxShaderGroupStride;

			uint64_t rayGenCount = 0;
			uint64_t hitGroupCount = 0;
			uint64_t missCount = 0;
			uint64_t callableCount = 0;

			uint64_t rayGenStride = 0;
			uint64_t missStride = 0;
			uint64_t hitGroupStride = 0;
			uint64_t callableStride = 0;

			// Get the allocation sizes.
			uint32_t groupCount = 0;
			for (const auto& group : bindingGroups)
			{
				for (const auto& [shader, entry] : group.m_Entries)
				{
					const auto entrySize = GetEntrySize(entry, handleSize, entryAlignment);
					groupCount++;

					switch (shader)
					{
					case ShaderType::RayGen:
						rayGenStride = std::max(entrySize, rayGenStride);
						rayGenCount++;
						break;

					case ShaderType::Intersection:
					case ShaderType::AnyHit:
					case ShaderType::ClosestHit:
						hitGroupStride = std::max(entrySize, hitGroupStride);
						hitGroupCount++;
						break;

					case ShaderType::Miss:
						missStride = std::max(entrySize, missStride);
						missCount++;
						break;

					case ShaderType::Callable:
						callableStride = std::max(entrySize, callableStride);
						callableCount++;
						break;

					default:
						XENON_LOG_ERROR("Invalid shader type provided to the shader builder! The only supported shader types are RayGen, Intersection, AnyHit, ClosestHit, Miss and Callable.");
						break;
					}
				}
			}

			// Validate the sizes.
			if (rayGenStride > maxStride || missStride > maxStride || hitGroupStride > maxStride || callableStride > maxStride)
			{
				XENON_LOG_ERROR("Failed to create the shader binding table! The size of the binding data are too much. The maximum size allowed is {}", maxStride * 4);
				return;
			}

			m_RayGenSize = rayGenCount * rayGenStride;
			m_RayHitSize = hitGroupCount * hitGroupStride;
			m_RayMissSize = missCount * missStride;
			m_CallableSize = callableCount * callableStride;

			const uint32_t handleSizeAligned = XENON_VK_ALIGNED_SIZE(handleSize, entryAlignment);
			const uint32_t sbtSize = groupCount * handleSizeAligned;

			std::vector<std::byte> shaderHandleStorage(sbtSize);
			XENON_VK_ASSERT(vkGetRayTracingShaderGroupHandlesKHR(m_pDevice->getLogicalDevice(), pPipeline->getPipeline(), 0, groupCount, sbtSize, shaderHandleStorage.data()));

			// Create the buffer.
			VkBufferCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.size = m_RayGenSize + m_RayMissSize + m_RayHitSize + m_CallableSize;
			createInfo.usage = VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
			createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;

			VmaAllocationCreateInfo allocationCreateInfo = {};
			allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
			allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;

			m_pDevice->getAllocator().access([this, createInfo, allocationCreateInfo](VmaAllocator allocator)
				{
					XENON_VK_ASSERT(vmaCreateBuffer(allocator, &createInfo, &allocationCreateInfo, &m_Table, &m_Allocation, nullptr), "Failed to shader binding table!");
				}
			);

			// Get the buffer memory.
			auto pRayGenMemory = map();
			auto pMissMemory = pRayGenMemory + m_RayGenSize;
			auto pHitGroupMemory = pMissMemory + m_RayMissSize;
			auto pCallableMemory = pHitGroupMemory + m_RayHitSize;

			// Copy the data.
			uint32_t index = 0;
			for (const auto& group : bindingGroups)
			{
				auto pBegin = shaderHandleStorage.data();
				for (const auto& [shaderType, entry] : group.m_Entries)
				{
					switch (shaderType)
					{
					case ShaderType::RayGen:
						CopyIncrement(pBegin, pRayGenMemory, handleSize);
						CopyEntry(entry, pRayGenMemory, rayGenStride);
						break;

					case ShaderType::Intersection:
					case ShaderType::AnyHit:
					case ShaderType::ClosestHit:
						CopyIncrement(pBegin, pHitGroupMemory, handleSize);
						CopyEntry(entry, pHitGroupMemory, hitGroupStride);
						break;

					case ShaderType::Miss:
						CopyIncrement(pBegin, pMissMemory, handleSize);
						CopyEntry(entry, pMissMemory, missStride);
						break;

					case ShaderType::Callable:
						CopyIncrement(pBegin, pCallableMemory, handleSize);
						CopyEntry(entry, pCallableMemory, callableStride);
						break;

					default:
						break;

					}

					pBegin += handleSize;
				}

				index++;
			}

			// Finally unmap the memory.
			unmap();

			// Setup the address ranges.
			m_RayGenAddressRegion.deviceAddress = getDeviceAddress();
			m_RayGenAddressRegion.stride = rayGenStride;
			m_RayGenAddressRegion.size = m_RayGenSize;

			m_MissAddressRegion.deviceAddress = m_RayGenAddressRegion.deviceAddress + m_RayGenAddressRegion.size;
			m_MissAddressRegion.stride = missStride;
			m_MissAddressRegion.size = m_RayMissSize;

			m_HitAddressRegion.deviceAddress = m_MissAddressRegion.deviceAddress + m_MissAddressRegion.size;
			m_HitAddressRegion.stride = hitGroupStride;
			m_HitAddressRegion.size = m_RayHitSize;

			m_CallableAddressRegion.deviceAddress = m_HitAddressRegion.deviceAddress + m_HitAddressRegion.size;
			m_CallableAddressRegion.stride = callableStride;
			m_CallableAddressRegion.size = m_CallableSize;
		}

		VulkanShaderBindingTable::~VulkanShaderBindingTable()
		{
			m_pDevice->getAllocator().access([this](VmaAllocator allocator) { vmaDestroyBuffer(allocator, m_Table, m_Allocation); });
		}

		std::byte* VulkanShaderBindingTable::map()
		{
			std::byte* pMemory = nullptr;
			m_pDevice->getAllocator().access([this, &pMemory](VmaAllocator allocator)
				{
					XENON_VK_ASSERT(vmaMapMemory(allocator, m_Allocation, std::bit_cast<void**>(&pMemory)), "Failed to map the shader bindng table memory!");
				}
			);

			return pMemory;
		}

		void VulkanShaderBindingTable::unmap()
		{
			m_pDevice->getAllocator().access([this](VmaAllocator allocator) { vmaUnmapMemory(allocator, m_Allocation); });
		}

		VkDeviceAddress VulkanShaderBindingTable::getDeviceAddress() const
		{
			VkBufferDeviceAddressInfoKHR bufferDeviceAddressInfo = {};
			bufferDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
			bufferDeviceAddressInfo.buffer = m_Table;
			return m_pDevice->getDeviceTable().vkGetBufferDeviceAddressKHR(m_pDevice->getLogicalDevice(), &bufferDeviceAddressInfo);
		}
	}
}