// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanComputePipeline.hpp"
#include "VulkanMacros.hpp"
#include "VulkanDescriptorSetManager.hpp"
#include "VulkanDescriptor.hpp"

#include <optick.h>

// This magic number is used by the rasterizing pipeline to uniquely identify it's pipeline caches.
constexpr auto g_MagicNumber = 0b0110010000111101101100100010100110111011101010111010111010000001;

namespace /* anonymous */
{
	/**
	 * Get the shader bindings.
	 *
	 * @param shader The shader to get the bindings from.
	 * @param bindingInfos The binding info structures.
	 * @param pushConstants The push constants vector to load the data.
	 */
	void GetShaderBindings(
		const Xenon::Backend::ShaderSource& shader,
		std::vector<Xenon::Backend::DescriptorBindingInfo>& bindingInfos,
		std::vector<VkPushConstantRange>& pushConstants)
	{
		// Get the resources.
		for (const auto& resource : shader.getResources())
		{
			auto& binding = bindingInfos.emplace_back();
			binding.m_Type = resource.m_Type;
			binding.m_ApplicableShaders = Xenon::Backend::ShaderType::Compute;
		}

		// // Get the buffers.
		// for (const auto& buffer : shader.getConstantBuffers())
		// {
		// 	auto& range = pushConstants.emplace_back();
		// 	range.offset = buffer.m_Offset;
		// 	range.size = buffer.m_Size;
		// 	range.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
		// }
	}
}

namespace Xenon
{
	namespace Backend
	{
		VulkanComputePipeline::VulkanComputePipeline(VulkanDevice* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, const ShaderSource& computeShader)
			: ComputePipeline(pDevice, std::move(pCacheHandler), computeShader)
			, VulkanDeviceBoundObject(pDevice)
		{
			// Get the shader information.
			std::vector<VkPushConstantRange> pushConstants;
			GetShaderBindings(computeShader, m_BindingInfos, pushConstants);

			// Generate the pipeline hash.
			m_PipelineHash = GenerateHash(ToBytes(computeShader.getBinary().data()), computeShader.getBinary().size());

			// Create the pipeline layout.
			createPipelineLayout(std::move(pushConstants));

			// Load the pipeline cache if possible.
			loadPipelineCache();

			// Create the pipeline.
			createPipeline();

			// Finally, store the pipeline cache.
			storePipelineCache();
		}

		VulkanComputePipeline::~VulkanComputePipeline()
		{
			m_pDevice->getDeviceTable().vkDestroyPipelineLayout(m_pDevice->getLogicalDevice(), m_PipelineLayout, nullptr);
			m_pDevice->getDeviceTable().vkDestroyPipelineCache(m_pDevice->getLogicalDevice(), m_PipelineCache, nullptr);
			m_pDevice->getDeviceTable().vkDestroyPipeline(m_pDevice->getLogicalDevice(), m_Pipeline, nullptr);
		}

		std::unique_ptr<Xenon::Backend::Descriptor> VulkanComputePipeline::createDescriptor(DescriptorType type)
		{
			OPTICK_EVENT();

			return std::make_unique<VulkanDescriptor>(m_pDevice, m_BindingInfos, DescriptorType::UserDefined);
		}

		void VulkanComputePipeline::createPipelineLayout(std::vector<VkPushConstantRange>&& pushConstants)
		{
			const auto descriptorSetLayout = m_pDevice->getDescriptorSetManager()->getDescriptorSetLayout(m_BindingInfos);

			VkPipelineLayoutCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.setLayoutCount = 1;
			createInfo.pSetLayouts = &descriptorSetLayout;
			createInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstants.size());
			createInfo.pPushConstantRanges = pushConstants.data();

			XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkCreatePipelineLayout(m_pDevice->getLogicalDevice(), &createInfo, nullptr, &m_PipelineLayout), "Failed to create the pipeline layout!");
		}

		void VulkanComputePipeline::loadPipelineCache()
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

		void VulkanComputePipeline::storePipelineCache()
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

		void VulkanComputePipeline::createPipeline()
		{
			OPTICK_EVENT();

			// Setup the shader module.
			VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
			shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStageCreateInfo.pNext = nullptr;
			shaderStageCreateInfo.flags = 0;
			shaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
			shaderStageCreateInfo.pName = "main";
			shaderStageCreateInfo.pSpecializationInfo = nullptr;

			VkShaderModuleCreateInfo moduleCreateInfo = {};
			moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			moduleCreateInfo.pNext = nullptr;
			moduleCreateInfo.flags = 0;
			moduleCreateInfo.codeSize = m_ComputeShaderSource.getBinary().size();
			moduleCreateInfo.pCode = m_ComputeShaderSource.getBinary().data();

			XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkCreateShaderModule(m_pDevice->getLogicalDevice(), &moduleCreateInfo, nullptr, &shaderStageCreateInfo.module), "Failed to create the compute shader module!");

			// Create the pipeline.
			VkComputePipelineCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.stage = shaderStageCreateInfo;
			createInfo.layout = m_PipelineLayout;
			createInfo.basePipelineHandle = VK_NULL_HANDLE;
			createInfo.basePipelineIndex = 0;

			XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkCreateComputePipelines(m_pDevice->getLogicalDevice(), m_PipelineCache, 1, &createInfo, nullptr, &m_Pipeline), "Failed to create the compute pipeline!");

			// Destroy the shader module.
			m_pDevice->getDeviceTable().vkDestroyShaderModule(m_pDevice->getLogicalDevice(), shaderStageCreateInfo.module, nullptr);
		}
	}
}