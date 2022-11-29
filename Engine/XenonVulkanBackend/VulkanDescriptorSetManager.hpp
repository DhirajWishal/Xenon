// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDevice.hpp"

#include <unordered_map>

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan descriptor set manager.
		 */
		class VulkanDescriptorSetManager final
		{
			/**
			 * Vulkan descriptor storage structure.
			 * This contains all the necessary information required by descriptor sets.
			 */
			struct VulkanDescriptorStorage final
			{
				std::vector<DescriptorBindingInfo> m_BindingInfo;
				std::vector<std::pair<VkDescriptorPool, uint32_t>> m_Pools;	// [Descriptor pool, active descriptor set count]
				VkDescriptorSetLayout m_Layout = VK_NULL_HANDLE;
			};

		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 */
			explicit VulkanDescriptorSetManager(VulkanDevice* pDevice);

			/**
			 * Destructor.
			 */
			~VulkanDescriptorSetManager();

			/**
			 * Get the descriptor set layout.
			 *
			 * @param bindingInfo The descriptor binding info.
			 * @param descriptorType The descriptor type.
			 * @return The descriptor set layout.
			 */
			[[nodicard]] VkDescriptorSetLayout getDescriptorSetLayout(const std::vector<DescriptorBindingInfo>& bindingInfo);

			/**
			 * Create a new descriptor set.
			 *
			 * @param bindingInfo The descriptor binding info.
			 * @param descriptorType The descriptor type.
			 * @return The descriptor pool and its set.
			 */
			[[nodiscard]] std::pair<VkDescriptorPool, VkDescriptorSet> createDescriptorSet(const std::vector<DescriptorBindingInfo>& bindingInfo);

			/**
			 * Free the descriptor set.
			 *
			 * @param pool The descriptor pool which owns the descriptor set.
			 * @param descriptorSet The descriptor set.
			 * @param bindingInfo The descriptor binding info.
			 * @param descriptorType The descriptor type.
			 */
			void freeDescriptorSet(VkDescriptorPool pool, VkDescriptorSet descriptorSet, const std::vector<DescriptorBindingInfo>& bindingInfo);

		private:
			std::unordered_map<uint64_t, VulkanDescriptorStorage> m_DescriptorSetStorages;

			VulkanDevice* m_pDevice = nullptr;
		};
	}
}