// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDeviceBoundObject.hpp"

#include <unordered_map>

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan descriptor set manager.
		 */
		class VulkanDescriptorSetManager : public VulkanDeviceBoundObject
		{
			/**
			 * Vulkan descriptor storage structure.
			 * This contains all the necessary information required by descriptor sets.
			 */
			struct VulkanDescriptorStorage final
			{
				std::unordered_map<uint32_t, DescriptorBindingInfo> m_BindingInfo;
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
			virtual ~VulkanDescriptorSetManager() override;

			/**
			 * Get the descriptor set layout.
			 *
			 * @param bindingInfo The descriptor binding info.
			 * @param descriptorType The descriptor type.
			 * @return The descriptor set layout.
			 */
			XENON_NODISCARD VkDescriptorSetLayout getDescriptorSetLayout(const std::unordered_map<uint32_t, DescriptorBindingInfo>& bindingInfo);

			/**
			 * Create a new descriptor set.
			 *
			 * @param bindingInfo The descriptor binding info.
			 * @param descriptorType The descriptor type.
			 * @return The descriptor pool and its set.
			 */
			XENON_NODISCARD std::pair<VkDescriptorPool, VkDescriptorSet> createDescriptorSet(const std::unordered_map<uint32_t, DescriptorBindingInfo>& bindingInfo);

			/**
			 * Free the descriptor set.
			 *
			 * @param pool The descriptor pool which owns the descriptor set.
			 * @param descriptorSet The descriptor set.
			 * @param bindingInfo The descriptor binding info.
			 * @param descriptorType The descriptor type.
			 */
			void freeDescriptorSet(VkDescriptorPool pool, VkDescriptorSet descriptorSet, const std::unordered_map<uint32_t, DescriptorBindingInfo>& bindingInfo);

		private:
			/**
			 * Generate a hash for the binding infos.
			 *
			 * @param bindingInfo The binding infos.
			 * @return The generated hash.
			 */
			XENON_NODISCARD uint64_t getBindingInfoHash(const std::unordered_map<uint32_t, DescriptorBindingInfo>& bindingInfo) const;

		private:
			std::unordered_map<uint64_t, VulkanDescriptorStorage> m_DescriptorSetStorages;

			VkDescriptorSetLayout m_DummyDescriptorSetLayout = VK_NULL_HANDLE;
			VkDescriptorPool m_DummyDescriptorPool = VK_NULL_HANDLE;
			VkDescriptorSet m_DummyDescriptorSet = VK_NULL_HANDLE;
		};
	}
}