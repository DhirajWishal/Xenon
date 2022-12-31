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
			/**
			 * Shader binding table structure.
			 */
			struct ShaderBindingTable final
			{
				VkStridedDeviceAddressRegionKHR m_AddressRegion = {};
				std::unique_ptr<VulkanBuffer> m_pTable = nullptr;
			};

		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param pCacheHandler The cache handler pointer. This can be null in which case the pipeline creation might get slow.
			 * @param shaderGroups The shader groups.
			 * @param maxRayRecursion The maximum ray recursion depth. Default is 4.
			 */
			explicit VulkanRayTracingPipeline(VulkanDevice* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, const std::vector<ShaderGroup>& shaderGroups, uint32_t maxRayRecursion = 4);

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
			 * Create a shader binding table.
			 *
			 * @param recordCount The number of records.
			 * @return The shader binding table.
			 */
			[[nodiscard]] ShaderBindingTable createShaderBindingTable(uint64_t recordCount) const;

			/**
			 * Create the pipeline.
			 *
			 * @param shaderStageCreateInfos The shader stage create info structures.
			 * @param shaderGroups The shader groups.
			 */
			void createPipeline(std::vector<VkPipelineShaderStageCreateInfo>&& shaderStageCreateInfos, std::vector<VkRayTracingShaderGroupCreateInfoKHR>&& shaderGroups);

			/**
			 * Write the shader group handles.
			 * If the buffer pointer is nullptr, it'll ignore.
			 *
			 * @param pBuffer The buffer pointer to copy to.
			 * @param pData The data pointer. This value will be incremented after writing the data.
			 * @param handleSize The handle size.
			 * @param handleSizeAligned The aligned handle size.
			 */
			void writeShaderGroupHandles(VulkanBuffer* pBuffer, std::byte*& pData, uint64_t handleSize, uint64_t handleSizeAligned) const;

			/**
			 * Prepare the shader binding tables.
			 */
			void prepareShaderBindingTables();

		private:
			std::unordered_map<DescriptorType, std::vector<DescriptorBindingInfo>> m_BindingMap;

			ShaderBindingTable m_RayGenBindingTable;
			ShaderBindingTable m_IntersectionBindingTable;
			ShaderBindingTable m_AnyHitBindingTable;
			ShaderBindingTable m_ClosestHitBindingTable;
			ShaderBindingTable m_MissBindingTable;
			ShaderBindingTable m_CallableBindingTable;

			VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
			VkPipelineCache m_PipelineCache = VK_NULL_HANDLE;
			VkPipeline m_Pipeline = VK_NULL_HANDLE;

			uint64_t m_PipelineHash = 0;
		};
	}
}