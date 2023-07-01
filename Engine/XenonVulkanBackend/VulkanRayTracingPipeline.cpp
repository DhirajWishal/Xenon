// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanRayTracingPipeline.hpp"
#include "VulkanMacros.hpp"
#include "VulkanDescriptorSetManager.hpp"
#include "VulkanDescriptor.hpp"
#include "VulkanShaderBindingTable.hpp"

#include <optick.h>

// This magic number is used by the ray tracing pipeline to uniquely identify it's pipeline caches.
constexpr uint64_t g_MagicNumber = 0b0010010010111100111000101101010101000110100101011100011100101000;

namespace /* anonymous */
{
	/**
	 * Get the shader bindings.
	 *
	 * @param source The shader source.
	 * @param bindingMap The shader's binding map.
	 * @param type The shader type.
	 */
	void GetShaderBindings(
		const Xenon::Backend::Shader& source,
		std::unordered_map<Xenon::Backend::DescriptorType, std::unordered_map<uint32_t, Xenon::Backend::DescriptorBindingInfo>>& bindingMap,
		Xenon::Backend::ShaderType type)
	{
		// Get the resources.
		for (const auto& resource : source.getResources())
		{
			auto& bindings = bindingMap[static_cast<Xenon::Backend::DescriptorType>(Xenon::EnumToInt(resource.m_Set))];

			if (bindings.contains(resource.m_Binding))
			{
				bindings[resource.m_Binding].m_ApplicableShaders |= type;
			}
			else
			{
				auto& binding = bindings[resource.m_Binding];
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
		VulkanRayTracingPipeline::VulkanRayTracingPipeline(VulkanDevice* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, const RayTracingPipelineSpecification& specification)
			: RayTracingPipeline(pDevice, std::move(pCacheHandler), specification)
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

			std::vector<uint64_t> shaderHashes = { specification.m_MaxRayRecursionDepth, specification.m_MaxPayloadSize, specification.m_MaxAttributeSize };
			for (const auto& group : specification.m_ShaderGroups)
			{
				auto& vkShaderGroup = vkShaderGroups.emplace_back();
				vkShaderGroup.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
				vkShaderGroup.generalShader = VK_SHADER_UNUSED_KHR;
				vkShaderGroup.closestHitShader = VK_SHADER_UNUSED_KHR;
				vkShaderGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
				vkShaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;

				if (group.m_RayGenShader.getSPIRV().isValid())
				{
					GetShaderBindings(group.m_RayGenShader, m_BindingMap, ShaderType::RayGen);
					shaderHashes.emplace_back(GenerateHash(ToBytes(group.m_RayGenShader.getSPIRV().getBinaryData()), group.m_RayGenShader.getSPIRV().getBinarySizeInBytes()));
					shaderStages.emplace_back(createShaderStage(group.m_RayGenShader, VK_SHADER_STAGE_RAYGEN_BIT_KHR));
					vkShaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
					vkShaderGroup.generalShader = static_cast<uint32_t>(shaderStages.size()) - 1;
					rayGenCount++;
				}

				if (group.m_IntersectionShader.getSPIRV().isValid())
				{
					GetShaderBindings(group.m_IntersectionShader, m_BindingMap, ShaderType::Intersection);
					shaderHashes.emplace_back(GenerateHash(ToBytes(group.m_IntersectionShader.getSPIRV().getBinaryData()), group.m_IntersectionShader.getSPIRV().getBinarySizeInBytes()));
					shaderStages.emplace_back(createShaderStage(group.m_IntersectionShader, VK_SHADER_STAGE_INTERSECTION_BIT_KHR));
					vkShaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT_GROUP_KHR;
					vkShaderGroup.intersectionShader = static_cast<uint32_t>(shaderStages.size()) - 1;
					intersectionCount++;
				}

				if (group.m_AnyHitShader.getSPIRV().isValid())
				{
					GetShaderBindings(group.m_AnyHitShader, m_BindingMap, ShaderType::AnyHit);
					shaderHashes.emplace_back(GenerateHash(ToBytes(group.m_AnyHitShader.getSPIRV().getBinaryData()), group.m_AnyHitShader.getSPIRV().getBinarySizeInBytes()));
					shaderStages.emplace_back(createShaderStage(group.m_AnyHitShader, VK_SHADER_STAGE_ANY_HIT_BIT_KHR));
					vkShaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
					vkShaderGroup.anyHitShader = static_cast<uint32_t>(shaderStages.size()) - 1;
					anyHitCount++;
				}

				if (group.m_ClosestHitShader.getSPIRV().isValid())
				{
					GetShaderBindings(group.m_ClosestHitShader, m_BindingMap, ShaderType::ClosestHit);
					shaderHashes.emplace_back(GenerateHash(ToBytes(group.m_ClosestHitShader.getSPIRV().getBinaryData()), group.m_ClosestHitShader.getSPIRV().getBinarySizeInBytes()));
					shaderStages.emplace_back(createShaderStage(group.m_ClosestHitShader, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR));
					vkShaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
					vkShaderGroup.closestHitShader = static_cast<uint32_t>(shaderStages.size()) - 1;
					closestHitCount++;
				}

				if (group.m_MissShader.getSPIRV().isValid())
				{
					GetShaderBindings(group.m_MissShader, m_BindingMap, ShaderType::Miss);
					shaderHashes.emplace_back(GenerateHash(ToBytes(group.m_MissShader.getSPIRV().getBinaryData()), group.m_MissShader.getSPIRV().getBinarySizeInBytes()));
					shaderStages.emplace_back(createShaderStage(group.m_MissShader, VK_SHADER_STAGE_MISS_BIT_KHR));
					vkShaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
					vkShaderGroup.generalShader = static_cast<uint32_t>(shaderStages.size()) - 1;
					missCount++;
				}

				if (group.m_CallableShader.getSPIRV().isValid())
				{
					GetShaderBindings(group.m_CallableShader, m_BindingMap, ShaderType::Callable);
					shaderHashes.emplace_back(GenerateHash(ToBytes(group.m_CallableShader.getSPIRV().getBinaryData()), group.m_CallableShader.getSPIRV().getBinarySizeInBytes()));
					shaderStages.emplace_back(createShaderStage(group.m_CallableShader, VK_SHADER_STAGE_CALLABLE_BIT_KHR));
					vkShaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
					vkShaderGroup.generalShader = static_cast<uint32_t>(shaderStages.size()) - 1;
					callableCount++;
				}
			}

			// Get the pipeline hash.
			m_PipelineHash = GenerateHash(ToBytes(shaderHashes.data()), sizeof(uint64_t) * shaderHashes.size());

			// Get the layouts.
			const std::array<VkDescriptorSetLayout, 4> layouts = {
				pDevice->getDescriptorSetManager()->getDescriptorSetLayout(m_BindingMap[DescriptorType::UserDefined]),
				pDevice->getDescriptorSetManager()->getDescriptorSetLayout(m_BindingMap[DescriptorType::Material]),
				pDevice->getDescriptorSetManager()->getDescriptorSetLayout(m_BindingMap[DescriptorType::PerGeometry]),
				pDevice->getDescriptorSetManager()->getDescriptorSetLayout(m_BindingMap[DescriptorType::Scene])
			};

			// Create the pipeline layout.
			createPipelineLayout(layouts, {});

			// Load the pipeline cache.
			loadPipelineCache();

			// Setup the pipeline here.
			createPipeline(std::move(shaderStages), std::move(vkShaderGroups));

			// Store the pipeline cache.
			storePipelineCache();
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

		std::unique_ptr<ShaderBindingTable> VulkanRayTracingPipeline::createShaderBindingTable(const std::vector<BindingGroup>& bindingGroups)
		{
			OPTICK_EVENT();
			return std::make_unique<VulkanShaderBindingTable>(m_pDevice, this, bindingGroups);
		}

		void VulkanRayTracingPipeline::createPipelineLayout(const std::array<VkDescriptorSetLayout, 4>& layouts, std::vector<VkPushConstantRange>&& pushConstantRanges)
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
			shaderModuleCreateInfo.codeSize = source.getSPIRV().getBinarySizeInBytes();
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
	}
}