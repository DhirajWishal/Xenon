// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanRayTracingPipeline.hpp"
#include "VulkanMacros.hpp"
#include "VulkanDescriptorSetManager.hpp"
#include "VulkanDescriptor.hpp"

#include <optick.h>

// This magic number is used by the ray tracing pipeline to uniquely identify it's pipeline caches.
constexpr uint64_t g_MagicNumber = 0b0010010010111100111000101101010101000110100101011100011100101000;

namespace /* anonymous */
{
	/**
	 * Get the shader stage flag bit from the Xenon shader type.
	 *
	 * @param shaderType The shader type.
	 * @return The Vulkan shader stage flag bit.
	 */
	[[nodiscard]] constexpr VkShaderStageFlagBits GetShaderStageFlagBit(Xenon::Backend::ShaderType shaderType) noexcept
	{
		switch (shaderType)
		{
		case Xenon::Backend::ShaderType::Vertex:
			return VK_SHADER_STAGE_VERTEX_BIT;

		case Xenon::Backend::ShaderType::Fragment:
			return VK_SHADER_STAGE_FRAGMENT_BIT;

		case Xenon::Backend::ShaderType::RayGen:
			return VK_SHADER_STAGE_RAYGEN_BIT_KHR;

		case Xenon::Backend::ShaderType::Intersection:
			return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;

		case Xenon::Backend::ShaderType::AnyHit:
			return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;

		case Xenon::Backend::ShaderType::ClosestHit:
			return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;

		case Xenon::Backend::ShaderType::Miss:
			return VK_SHADER_STAGE_MISS_BIT_KHR;

		case Xenon::Backend::ShaderType::Callable:
			return VK_SHADER_STAGE_CALLABLE_BIT_KHR;

		case Xenon::Backend::ShaderType::Compute:
			return VK_SHADER_STAGE_COMPUTE_BIT;

		default:
			XENON_LOG_ERROR("Invalid shader type provided! Defaulting to All.");
			return VK_SHADER_STAGE_ALL;
		}
	}

	/**
	 * Get the shader bindings.
	 *
	 * @param source The shader source.
	 * @param bindingMap The shader's binding map.
	 * @param indexToBindingMap The index to binding map.
	 * @param type The shader type.
	 */
	void GetShaderBindings(
		const Xenon::Backend::Shader& source,
		std::unordered_map<Xenon::Backend::DescriptorType, std::vector<Xenon::Backend::DescriptorBindingInfo>>& bindingMap,
		std::unordered_map<uint32_t, std::unordered_map<uint32_t, size_t>>& indexToBindingMap,
		Xenon::Backend::ShaderType type)
	{
		// Get the resources.
		for (const auto& resource : source.getResources())
		{
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
		}
	}
}

namespace Xenon
{
	namespace Backend
	{
		VulkanRayTracingPipeline::VulkanRayTracingPipeline(VulkanDevice* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, const std::vector<ShaderGroup>& shaderGroups, uint32_t maxRayRecursion /*= 4*/)
			: RayTracingPipeline(pDevice, std::move(pCacheHandler), shaderGroups, maxRayRecursion)
			, VulkanDeviceBoundObject(pDevice)
		{
			OPTICK_EVENT();

			// Resolve shader groups.
			uint64_t rayGenCount = 0;
			uint64_t intersectionCount = 0;
			uint64_t anyHitCount = 0;
			uint64_t closestHitCount = 0;
			uint64_t missCount = 0;
			uint64_t callableCount = 0;

			std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
			std::vector<VkRayTracingShaderGroupCreateInfoKHR> vkShaderGroups;

			std::unordered_map<uint32_t, std::unordered_map<uint32_t, size_t>> indexToBindingMap;

			std::vector<uint64_t> shaderHashes = { maxRayRecursion };
			for (const auto& group : shaderGroups)
			{
				auto& vkShaderGroup = vkShaderGroups.emplace_back();
				vkShaderGroup.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
				vkShaderGroup.generalShader = VK_SHADER_UNUSED_KHR;
				vkShaderGroup.closestHitShader = VK_SHADER_UNUSED_KHR;
				vkShaderGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
				vkShaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;

				if (group.m_RayGenShader.getSPIRV().isValid())
				{
					GetShaderBindings(group.m_RayGenShader, m_BindingMap, indexToBindingMap, ShaderType::RayGen);
					shaderHashes.emplace_back(GenerateHash(ToBytes(group.m_RayGenShader.getSPIRV().getBinaryData()), group.m_RayGenShader.getSPIRV().getBinarySizeInBytes()));
					shaderStages.emplace_back(createShaderStage(group.m_RayGenShader, VK_SHADER_STAGE_RAYGEN_BIT_KHR));
					vkShaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
					vkShaderGroup.generalShader = static_cast<uint32_t>(shaderStages.size()) - 1;
					rayGenCount++;
				}

				if (group.m_IntersectionShader.getSPIRV().isValid())
				{
					GetShaderBindings(group.m_IntersectionShader, m_BindingMap, indexToBindingMap, ShaderType::Intersection);
					shaderHashes.emplace_back(GenerateHash(ToBytes(group.m_IntersectionShader.getSPIRV().getBinaryData()), group.m_IntersectionShader.getSPIRV().getBinarySizeInBytes()));
					shaderStages.emplace_back(createShaderStage(group.m_IntersectionShader, VK_SHADER_STAGE_INTERSECTION_BIT_KHR));
					vkShaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT_GROUP_KHR;
					vkShaderGroup.intersectionShader = static_cast<uint32_t>(shaderStages.size()) - 1;
					intersectionCount++;
				}

				if (group.m_AnyHitShader.getSPIRV().isValid())
				{
					GetShaderBindings(group.m_AnyHitShader, m_BindingMap, indexToBindingMap, ShaderType::AnyHit);
					shaderHashes.emplace_back(GenerateHash(ToBytes(group.m_AnyHitShader.getSPIRV().getBinaryData()), group.m_AnyHitShader.getSPIRV().getBinarySizeInBytes()));
					shaderStages.emplace_back(createShaderStage(group.m_AnyHitShader, VK_SHADER_STAGE_ANY_HIT_BIT_KHR));
					vkShaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
					vkShaderGroup.anyHitShader = static_cast<uint32_t>(shaderStages.size()) - 1;
					anyHitCount++;
				}

				if (group.m_ClosestHitShader.getSPIRV().isValid())
				{
					GetShaderBindings(group.m_ClosestHitShader, m_BindingMap, indexToBindingMap, ShaderType::ClosestHit);
					shaderHashes.emplace_back(GenerateHash(ToBytes(group.m_ClosestHitShader.getSPIRV().getBinaryData()), group.m_ClosestHitShader.getSPIRV().getBinarySizeInBytes()));
					shaderStages.emplace_back(createShaderStage(group.m_ClosestHitShader, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR));
					vkShaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
					vkShaderGroup.closestHitShader = static_cast<uint32_t>(shaderStages.size()) - 1;
					closestHitCount++;
				}

				if (group.m_MissShader.getSPIRV().isValid())
				{
					GetShaderBindings(group.m_MissShader, m_BindingMap, indexToBindingMap, ShaderType::Miss);
					shaderHashes.emplace_back(GenerateHash(ToBytes(group.m_MissShader.getSPIRV().getBinaryData()), group.m_MissShader.getSPIRV().getBinarySizeInBytes()));
					shaderStages.emplace_back(createShaderStage(group.m_MissShader, VK_SHADER_STAGE_MISS_BIT_KHR));
					vkShaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
					vkShaderGroup.generalShader = static_cast<uint32_t>(shaderStages.size()) - 1;
					missCount++;
				}

				if (group.m_CallableShader.getSPIRV().isValid())
				{
					GetShaderBindings(group.m_CallableShader, m_BindingMap, indexToBindingMap, ShaderType::Callable);
					shaderHashes.emplace_back(GenerateHash(ToBytes(group.m_CallableShader.getSPIRV().getBinaryData()), group.m_CallableShader.getSPIRV().getBinarySizeInBytes()));
					shaderStages.emplace_back(createShaderStage(group.m_CallableShader, VK_SHADER_STAGE_CALLABLE_BIT_KHR));
					vkShaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
					vkShaderGroup.generalShader = static_cast<uint32_t>(shaderStages.size()) - 1;
					callableCount++;
				}
			}

			// Get the pipeline hash.
			m_PipelineHash = GenerateHash(ToBytes(shaderHashes.data()), sizeof(uint64_t) * shaderHashes.size());

			// Setup the binding tables.
			m_RayGenBindingTable = createShaderBindingTable(rayGenCount);
			m_IntersectionBindingTable = createShaderBindingTable(intersectionCount);
			m_AnyHitBindingTable = createShaderBindingTable(anyHitCount);
			m_ClosestHitBindingTable = createShaderBindingTable(closestHitCount);
			m_MissBindingTable = createShaderBindingTable(missCount);
			m_CallableBindingTable = createShaderBindingTable(callableCount);

			// Setup any missing bindings.
			if (!m_BindingMap.contains(DescriptorType::UserDefined)) m_BindingMap[DescriptorType::UserDefined];
			if (!m_BindingMap.contains(DescriptorType::Material)) m_BindingMap[DescriptorType::Material];
			if (!m_BindingMap.contains(DescriptorType::Camera)) m_BindingMap[DescriptorType::Camera];

			// Sort the bindings to the correct binding order.
			auto sortedBindings = std::vector<std::pair<DescriptorType, std::vector<DescriptorBindingInfo>>>(m_BindingMap.begin(), m_BindingMap.end());
			std::ranges::sort(sortedBindings, [](const auto& lhs, const auto& rhs) { return EnumToInt(lhs.first) < EnumToInt(rhs.first); });

			// Get the layouts.
			std::vector<VkDescriptorSetLayout> layouts;
			for (const auto& [set, bindings] : sortedBindings)
				layouts.emplace_back(pDevice->getDescriptorSetManager()->getDescriptorSetLayout(bindings));

			// Create the pipeline layout.
			createPipelineLayout(std::move(layouts), {});

			// Load the pipeline cache.
			loadPipelineCache();

			// Setup the pipeline here.
			createPipeline(std::move(shaderStages), std::move(vkShaderGroups));

			// Store the pipeline cache.
			storePipelineCache();

			// Prepare the shader binding tables.
			prepareShaderBindingTables();
		}

		VulkanRayTracingPipeline::~VulkanRayTracingPipeline()
		{
			m_pDevice->getDeviceTable().vkDestroyPipelineLayout(m_pDevice->getLogicalDevice(), m_PipelineLayout, nullptr);
			m_pDevice->getDeviceTable().vkDestroyPipelineCache(m_pDevice->getLogicalDevice(), m_PipelineCache, nullptr);
			m_pDevice->getDeviceTable().vkDestroyPipeline(m_pDevice->getLogicalDevice(), m_Pipeline, nullptr);
		}

		std::unique_ptr<Xenon::Backend::Descriptor> VulkanRayTracingPipeline::createDescriptor(DescriptorType type)
		{
			OPTICK_EVENT();
			return std::make_unique<VulkanDescriptor>(m_pDevice, m_BindingMap[type], type);
		}

		void VulkanRayTracingPipeline::createPipelineLayout(std::vector<VkDescriptorSetLayout>&& layouts, std::vector<VkPushConstantRange>&& pushConstantRanges)
		{
			VkPipelineLayoutCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());
			createInfo.pSetLayouts = layouts.data();
			createInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
			createInfo.pPushConstantRanges = pushConstantRanges.data();

			XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkCreatePipelineLayout(m_pDevice->getLogicalDevice(), &createInfo, nullptr, &m_PipelineLayout), "Failed to create the pipeline layout!");
		}

		void VulkanRayTracingPipeline::loadPipelineCache()
		{
			OPTICK_EVENT();

			std::vector<std::byte> cacheData;
			if (m_pCacheHandler)
				cacheData = m_pCacheHandler->load(m_PipelineHash ^ g_MagicNumber);

			else
				XENON_LOG_INFORMATION("A pipeline cache handler was not set to load the pipeline cache.");

			VkPipelineCacheCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.initialDataSize = cacheData.size();
			createInfo.pInitialData = cacheData.data();

			XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkCreatePipelineCache(m_pDevice->getLogicalDevice(), &createInfo, nullptr, &m_PipelineCache), "Failed to load the pipeline cache!");
		}

		void VulkanRayTracingPipeline::storePipelineCache()
		{
			OPTICK_EVENT();

			if (m_pCacheHandler)
			{
				size_t cacheSize = 0;
				XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkGetPipelineCacheData(m_pDevice->getLogicalDevice(), m_PipelineCache, &cacheSize, nullptr), "Failed to get the pipeline cache size!");

				auto cacheData = std::vector<std::byte>(cacheSize);
				XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkGetPipelineCacheData(m_pDevice->getLogicalDevice(), m_PipelineCache, &cacheSize, cacheData.data()), "Failed to get the pipeline cache data!");

				m_pCacheHandler->store(m_PipelineHash ^ g_MagicNumber, cacheData);
			}
			else
			{
				XENON_LOG_INFORMATION("A pipeline cache handler was not set to save the pipeline cache.");
			}
		}

		VkPipelineShaderStageCreateInfo VulkanRayTracingPipeline::createShaderStage(const Shader& source, VkShaderStageFlagBits shaderStage) const
		{
			OPTICK_EVENT();

			VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
			shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			shaderModuleCreateInfo.pNext = nullptr;
			shaderModuleCreateInfo.flags = 0;
			shaderModuleCreateInfo.codeSize = source.getSPIRV().getBinarySize();
			shaderModuleCreateInfo.pCode = source.getSPIRV().getBinaryData();

			VkShaderModule shaderModule = VK_NULL_HANDLE;
			XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkCreateShaderModule(m_pDevice->getLogicalDevice(), &shaderModuleCreateInfo, nullptr, &shaderModule), "Failed to create the ray tracing shader module!");

			VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
			shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStageCreateInfo.pNext = nullptr;
			shaderStageCreateInfo.flags = 0;
			shaderStageCreateInfo.stage = shaderStage;
			shaderStageCreateInfo.module = shaderModule;
			shaderStageCreateInfo.pName = source.getSPIRV().getEntryPoint().data();
			shaderStageCreateInfo.pSpecializationInfo = VK_NULL_HANDLE;

			return shaderStageCreateInfo;
		}

		Xenon::Backend::VulkanRayTracingPipeline::ShaderBindingTable VulkanRayTracingPipeline::createShaderBindingTable(uint64_t recordCount) const
		{
			OPTICK_EVENT();

			// Return if we don't have any records.
			if (recordCount == 0)
				return {};

			// Else prepare the table.
			const auto handleSize = m_pDevice->getPhysicalDeviceRayTracingPipelineProperties().shaderGroupHandleSize;
			const auto handleAlignment = m_pDevice->getPhysicalDeviceRayTracingPipelineProperties().shaderGroupHandleAlignment;

			ShaderBindingTable bindingTable = {};
			bindingTable.m_pTable = std::make_unique<VulkanBuffer>(
				m_pDevice,
				handleSize * recordCount,
				VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
				VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
				VMA_MEMORY_USAGE_AUTO_PREFER_HOST);

			const uint64_t stride = XENON_VK_ALIGNED_SIZE(handleSize, handleAlignment);

			bindingTable.m_AddressRegion.deviceAddress = bindingTable.m_pTable->getDeviceAddress();
			bindingTable.m_AddressRegion.stride = stride;
			bindingTable.m_AddressRegion.size = stride * recordCount;

			return bindingTable;
		}

		void VulkanRayTracingPipeline::createPipeline(std::vector<VkPipelineShaderStageCreateInfo>&& shaderStageCreateInfos, std::vector<VkRayTracingShaderGroupCreateInfoKHR>&& shaderGroups)
		{
			OPTICK_EVENT();

			VkRayTracingPipelineCreateInfoKHR createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.stageCount = static_cast<uint32_t>(shaderStageCreateInfos.size());
			createInfo.pStages = shaderStageCreateInfos.data();
			createInfo.groupCount = static_cast<uint32_t>(shaderGroups.size());
			createInfo.pGroups = shaderGroups.data();
			createInfo.maxPipelineRayRecursionDepth = getMaximumRayRecursionDepth();
			createInfo.layout = m_PipelineLayout;

			XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkCreateRayTracingPipelinesKHR(m_pDevice->getLogicalDevice(), VK_NULL_HANDLE, m_PipelineCache, 1, &createInfo, nullptr, &m_Pipeline), "Failed to create the ray tracing pipeline!");

			// Destroy the previously created shaders.
			for (const auto& stage : shaderStageCreateInfos)
				m_pDevice->getDeviceTable().vkDestroyShaderModule(m_pDevice->getLogicalDevice(), stage.module, nullptr);
		}

		void VulkanRayTracingPipeline::writeShaderGroupHandles(VulkanBuffer* pBuffer, std::byte*& pData, uint64_t handleSize, uint64_t handleSizeAligned) const
		{
			OPTICK_EVENT();

			if (pBuffer == nullptr)
				return;

			pBuffer->write(pData, handleSize);
			pData += handleSizeAligned;
		}

		void VulkanRayTracingPipeline::prepareShaderBindingTables()
		{
			OPTICK_EVENT();

			const auto handleSize = m_pDevice->getPhysicalDeviceRayTracingPipelineProperties().shaderGroupHandleSize;
			const auto handleAlignment = m_pDevice->getPhysicalDeviceRayTracingPipelineProperties().shaderGroupHandleAlignment;

			const uint32_t handleSizeAligned = XENON_VK_ALIGNED_SIZE(handleSize, handleAlignment);;
			const auto groupCount = static_cast<uint32_t>(m_ShaderGroups.size());
			const uint32_t sbtSize = groupCount * handleSizeAligned;

			std::vector<std::byte> shaderHandleStorage(sbtSize);
			XENON_VK_ASSERT(vkGetRayTracingShaderGroupHandlesKHR(m_pDevice->getLogicalDevice(), m_Pipeline, 0, groupCount, sbtSize, shaderHandleStorage.data()));

			auto ptr = shaderHandleStorage.data();
			writeShaderGroupHandles(m_RayGenBindingTable.m_pTable.get(), ptr, handleSize, handleSizeAligned);
			writeShaderGroupHandles(m_IntersectionBindingTable.m_pTable.get(), ptr, handleSize, handleSizeAligned);
			writeShaderGroupHandles(m_AnyHitBindingTable.m_pTable.get(), ptr, handleSize, handleSizeAligned);
			writeShaderGroupHandles(m_ClosestHitBindingTable.m_pTable.get(), ptr, handleSize, handleSizeAligned);
			writeShaderGroupHandles(m_MissBindingTable.m_pTable.get(), ptr, handleSize, handleSizeAligned);
			writeShaderGroupHandles(m_CallableBindingTable.m_pTable.get(), ptr, handleSize, handleSizeAligned);
		}
	}
}