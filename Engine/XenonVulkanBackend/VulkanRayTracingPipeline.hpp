// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/RayTracingPipeline.hpp"

#include "VulkanBuffer.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan ray tracing pipeline class.
		 */
		class VulkanRayTracingPipeline final : public RayTracingPipeline, public VulkanDeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param pCacheHandler The cache handler pointer. This can be null in which case the pipeline creation might get slow.
			 * @param specification The pipeline specification.
			 */
			explicit VulkanRayTracingPipeline(VulkanDevice* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, const RayTracingPipelineSpecification& specification);

			/**
			 * Destructor.
			 */
			~VulkanRayTracingPipeline() override;

			/**
			 * Create a new descriptor.
			 *
			 * @param type The descriptor type to create.
			 * @return The descriptor pointer. It will return nullptr if the descriptor type is not present in the pipeline.
			 */
			[[nodiscard]] std::unique_ptr<Descriptor> createDescriptor(DescriptorType type) override;

			/**
			 * Create a new ray generation shader binding table.
			 *
			 * @param bindingGroups The binding groups.
			 * @return The created shader binding table.
			 */
			[[nodiscard]] std::unique_ptr<ShaderBindingTable> createShaderBindingTable(const std::vector<BindingGroup>& bindingGroups) override;

			/**
			 * Get the pipeline.
			 *
			 * @return The pipeline handle.
			 */
			[[nodiscard]] VkPipeline getPipeline() const noexcept { return m_Pipeline; }

		private:
			/**
			 * Create the pipeline layout.
			 *
			 * @param layouts The descriptor set layouts.
			 * @param pushConstantRanges The push constant ranges.
			 */
			void createPipelineLayout(std::vector<VkDescriptorSetLayout>&& layouts, std::vector<VkPushConstantRange>&& pushConstantRanges);

			/**
			 * Load the pipeline cache from the cache handler.
			 */
			void loadPipelineCache();

			/**
			 * Store the pipeline cache using the cache handler.
			 */
			void storePipelineCache();

			/**
			 * Create the shader stage.
			 *
			 * @param source The shader.
			 * @param shaderStage The shader stage.
			 * @return The create info structure.
			 */
			[[nodiscard]] VkPipelineShaderStageCreateInfo createShaderStage(const Shader& source, VkShaderStageFlagBits shaderStage) const;

			/**
			 * Create the pipeline.
			 *
			 * @param shaderStageCreateInfos The shader stage create info structures.
			 * @param shaderGroups The shader groups.
			 */
			void createPipeline(std::vector<VkPipelineShaderStageCreateInfo>&& shaderStageCreateInfos, std::vector<VkRayTracingShaderGroupCreateInfoKHR>&& shaderGroups);

		private:
			std::unordered_map<DescriptorType, std::vector<DescriptorBindingInfo>> m_BindingMap;

			VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
			VkPipelineCache m_PipelineCache = VK_NULL_HANDLE;
			VkPipeline m_Pipeline = VK_NULL_HANDLE;

			uint64_t m_PipelineHash = 0;
		};
	}
}