// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/ComputePipeline.hpp"

#include "VulkanDeviceBoundObject.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan compute pipeline class.
		 */
		class VulkanComputePipeline final : public ComputePipeline, public VulkanDeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param pCacheHandler The cache handler pointer. This can be null in which case the pipeline creation might get slow.
			 * @param computeShader The compute shader.
			 */
			explicit VulkanComputePipeline(VulkanDevice* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, const Shader& computeShader);

			/**
			 * Destructor.
			 */
			~VulkanComputePipeline() override;

			/**
			 * Create a new descriptor.
			 *
			 * @param type The descriptor type to create.
			 * @return The descriptor pointer. It will return nullptr if the descriptor type is not present in the pipeline.
			 */
			[[nodiscard]] std::unique_ptr<Descriptor> createDescriptor(DescriptorType type) override;

			/**
			 * Get the pipeline.
			 *
			 * @return The pipeline handle.
			 */
			[[nodiscard]] VkPipeline getPipeline() const noexcept { return m_Pipeline; }

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
			 * @param pushConstants The push constants vector.
			 */
			void createPipelineLayout(std::vector<VkPushConstantRange>&& pushConstants);

			/**
			 * Load the pipeline cache from the cache handler.
			 */
			void loadPipelineCache();

			/**
			 * Store the pipeline cache using the cache handler.
			 */
			void storePipelineCache();

			/**
			 * Create the pipeline.
			 */
			void createPipeline();

		private:
			std::unordered_map<uint32_t, DescriptorBindingInfo> m_BindingInfos;

			VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
			VkPipelineCache m_PipelineCache = VK_NULL_HANDLE;
			VkPipeline m_Pipeline = VK_NULL_HANDLE;

			uint64_t m_PipelineHash = 0;
		};
	}
}