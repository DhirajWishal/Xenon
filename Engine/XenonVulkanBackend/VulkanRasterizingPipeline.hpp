// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/RasterizingPipeline.hpp"

#include "VulkanDeviceBoundObject.hpp"

namespace Xenon
{
	namespace Backend
	{
		class VulkanRasterizer;

		/**
		 * Vulkan rasterizing pipeline class.
		 */
		class VulkanRasterizingPipeline final : public RasterizingPipeline, public VulkanDeviceBoundObject
		{
			/**
			 * Pipeline storage structure.
			 */
			struct PipelineStorage final
			{
				std::vector<VkVertexInputBindingDescription> m_InputBindingDescriptions;
				std::vector<VkVertexInputAttributeDescription> m_InputAttributeDescriptions;

				VkPipeline m_Pipeline = VK_NULL_HANDLE;
				VkPipelineCache m_PipelineCache = VK_NULL_HANDLE;
			};

		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param pCacheHandler The cache handler pointer.
			 * @param pRasterizer The rasterizer pointer.
			 * @param specification The pipeline specification.
			 */
			explicit VulkanRasterizingPipeline(VulkanDevice* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, VulkanRasterizer* pRasterizer, const RasterizingPipelineSpecification& specification);

			/**
			 * Destructor.
			 */
			~VulkanRasterizingPipeline() override;

			/**
			 * Create a new descriptor.
			 *
			 * @param type The descriptor type to create.
			 * @return The descriptor pointer. It will return nullptr if the descriptor type is not present in the pipeline.
			 */
			[[nodiscard]] std::unique_ptr<Descriptor> createDescriptor(DescriptorType type) override;

			/**
			 * Get the pipeline.
			 * If a pipeline does not exist for the vertex specification, it will create a new one.
			 *
			 * @param vertexSpecification The vertex specification.
			 * @return The pipeline storage.
			 */
			const PipelineStorage& getPipeline(const VertexSpecification& vertexSpecification);

			/**
			 * Recreate the pipelines.
			 * This should be called if the associated rasterizer resizes.
			 */
			void recreate();

			/**
			 * Get the pipeline layout.
			 *
			 * @return The pipeline layout.
			 */
			[[nodiscard]] VkPipelineLayout getPipelineLayout() const noexcept { return m_PipelineLayout; }

		private:
			/**
			 * Create the pipeline layout.
			 *
			 * @param layouts The descriptor set layouts.
			 * @param pushConstantRanges The push constant ranges.
			 */
			void createPipelineLayout(std::vector<VkDescriptorSetLayout>&& layouts, std::vector<VkPushConstantRange>&& pushConstantRanges);

			/**
			 * Load the pipeline cache.
			 *
			 * @param hash The pipeline hash.
			 * @param pipeline The pipeline to load the cache to.
			 */
			void loadPipelineCache(uint64_t hash, PipelineStorage& pipeline) const;

			/**
			 * Save the pipeline cache.
			 *
			 * @param hash The pipeline hash.
			 * @param pipeline The pipeline to load the cache to.
			 */
			void savePipelineCache(uint64_t hash, PipelineStorage& pipeline) const;

			/**
			 * Setup the basic pipeline information.
			 * This information are setup once so that when needed, the pipeline-recreation process won't have to set these up again.
			 */
			void setupPipelineInfo();

			/**
			 * Create the required pipeline.
			 * This will automatically destroy the previous pipeline if a pipeline is defined within.
			 *
			 * @param pipeline The pipeline to create to.
			 */
			void createPipeline(PipelineStorage& pipeline) const;

		private:
			VkPipelineInputAssemblyStateCreateInfo m_InputAssemblyStateCreateInfo = {};
			VkPipelineTessellationStateCreateInfo m_TessellationStateCreateInfo = {};
			VkPipelineColorBlendStateCreateInfo m_ColorBlendStateCreateInfo = {};
			VkPipelineRasterizationStateCreateInfo m_RasterizationStateCreateInfo = {};
			VkPipelineMultisampleStateCreateInfo m_MultisampleStateCreateInfo = {};
			VkPipelineDepthStencilStateCreateInfo m_DepthStencilStateCreateInfo = {};
			VkPipelineDynamicStateCreateInfo m_DynamicStateCreateInfo = {};

			std::unordered_map<DescriptorType, std::vector<DescriptorBindingInfo>> m_BindingMap;
			std::unordered_map<uint64_t, PipelineStorage> m_Pipelines;

			std::vector<VkVertexInputBindingDescription> m_VertexInputBindings;
			std::vector<VkVertexInputAttributeDescription> m_VertexInputAttributes;
			std::vector<VkPipelineColorBlendAttachmentState> m_CBASS = {};
			std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStageCreateInfo = {};
			std::vector<VkDynamicState> m_DynamicStates = {};

			VulkanRasterizer* m_pRasterizer = nullptr;

			VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
		};
	}
}