// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanDescriptorSetManager.hpp"
#include "VulkanMacros.hpp"

namespace /* anonymous */
{
	/**
	 * Get all the shader stage flags defined in the type.
	 *
	 * @param type The type of the shader.
	 * @return The shader stage flags.
	 */
	VkShaderStageFlags GetStageFlags(Xenon::Backend::ShaderType type)
	{
		VkShaderStageFlags flags = 0;
		if (type & Xenon::Backend::ShaderType::Vertex) flags |= VK_SHADER_STAGE_VERTEX_BIT;
		if (type & Xenon::Backend::ShaderType::Fragment) flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
		if (type & Xenon::Backend::ShaderType::RayGen) flags |= VK_SHADER_STAGE_RAYGEN_BIT_KHR;
		if (type & Xenon::Backend::ShaderType::Intersection) flags |= VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
		if (type & Xenon::Backend::ShaderType::AnyHit) flags |= VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
		if (type & Xenon::Backend::ShaderType::ClosestHit) flags |= VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
		if (type & Xenon::Backend::ShaderType::Miss) flags |= VK_SHADER_STAGE_MISS_BIT_KHR;
		if (type & Xenon::Backend::ShaderType::Callable) flags |= VK_SHADER_STAGE_CALLABLE_BIT_KHR;
		if (type & Xenon::Backend::ShaderType::Compute) flags |= VK_SHADER_STAGE_COMPUTE_BIT;

		return flags;
	}
}

namespace Xenon
{
	namespace Backend
	{
		VulkanDescriptorSetManager::VulkanDescriptorSetManager(VulkanDevice* pDevice)
			: m_pDevice(pDevice)
		{
		}

		VulkanDescriptorSetManager::~VulkanDescriptorSetManager()
		{
			for (const auto& storages : m_DescriptorSetStorages)
			{
				for (const auto& [hash, storage] : storages)
				{
					for (const auto& [pool, count] : storage.m_Pools)
						m_pDevice->getDeviceTable().vkDestroyDescriptorPool(m_pDevice->getLogicalDevice(), pool, nullptr);

					m_pDevice->getDeviceTable().vkDestroyDescriptorSetLayout(m_pDevice->getLogicalDevice(), storage.m_Layout, nullptr);
				}
			}
		}

		std::pair<VkDescriptorPool, VkDescriptorSet> VulkanDescriptorSetManager::createDescriptorSet(const std::vector<DescriptorBindingInfo>& bindingInfo, DescriptorType descriptorType)
		{
			auto& descriptorStorage = m_DescriptorSetStorages[EnumToInt(descriptorType)];
			const auto bindingHash = GenerateHash(reinterpret_cast<const std::byte*>(bindingInfo.data()), bindingInfo.size() * sizeof(DescriptorBindingInfo));

			// Create a new one if the layout for the hash does not exist.
			if (!descriptorStorage.contains(bindingHash))
			{
				// Get the basic information from the binding info.
				std::vector<VkDescriptorSetLayoutBinding> bindings;
				std::vector<VkDescriptorPoolSize> poolSizes;
				bindings.reserve(bindingInfo.size());
				poolSizes.reserve(bindingInfo.size());

				for (uint32_t index = 0; index < bindingInfo.size(); index++)
				{
					const auto& binding = bindingInfo[index];

					auto& vkBinding = bindings.emplace_back();
					vkBinding.binding = index;
					vkBinding.descriptorCount = 1;
					vkBinding.descriptorType = m_pDevice->convertResourceType(binding.m_Type);
					vkBinding.pImmutableSamplers = nullptr;
					vkBinding.stageFlags = GetStageFlags(binding.m_ApplicableShaders);

					auto& vkPoolSize = poolSizes.emplace_back();
					vkPoolSize.descriptorCount = 1;
					vkPoolSize.type = vkBinding.descriptorType;
				}

				// Create the descriptor set layout.
				VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
				layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				layoutCreateInfo.pNext = nullptr;
				layoutCreateInfo.flags = 0;
				layoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
				layoutCreateInfo.pBindings = bindings.data();

				VkDescriptorSetLayout layout = VK_NULL_HANDLE;
				XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkCreateDescriptorSetLayout(m_pDevice->getLogicalDevice(), &layoutCreateInfo, nullptr, &layout), "Failed to create the descriptor set layout!");

				// Create the first descriptor pool.
				VkDescriptorPoolCreateInfo poolCreateInfo = {};
				poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
				poolCreateInfo.pNext = nullptr;
				poolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
				poolCreateInfo.maxSets = XENON_VK_MAX_DESCRIPTOR_SETS_COUNT;
				poolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
				poolCreateInfo.pPoolSizes = poolSizes.data();

				VkDescriptorPool pool = VK_NULL_HANDLE;
				XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkCreateDescriptorPool(m_pDevice->getLogicalDevice(), &poolCreateInfo, nullptr, &pool), "Failed to create the descriptor pool!");

				// Set the information to the storage.
				auto& storage = descriptorStorage[bindingHash];
				storage.m_BindingInfo = bindingInfo;
				storage.m_Layout = layout;
				storage.m_Pools.emplace_back(pool, 0);
			}

			auto& storage = descriptorStorage[bindingHash];

			// Find a suitable descriptor set.
			VkDescriptorPool pool = VK_NULL_HANDLE;
			for (auto& [descriptorPool, count] : storage.m_Pools)
			{
				if (count < XENON_VK_MAX_DESCRIPTOR_SETS_COUNT)
				{
					pool = descriptorPool;
					count++;
					break;
				}
			}

			// If we were not able to select a pool, create a new one.
			if (pool == VK_NULL_HANDLE)
			{
				std::vector<VkDescriptorPoolSize> poolSizes;
				poolSizes.reserve(bindingInfo.size());

				for (const auto& binding : bindingInfo)
				{
					auto& vkPoolSize = poolSizes.emplace_back();
					vkPoolSize.descriptorCount = 1;
					vkPoolSize.type = m_pDevice->convertResourceType(binding.m_Type);
				}

				VkDescriptorPoolCreateInfo poolCreateInfo = {};
				poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
				poolCreateInfo.pNext = nullptr;
				poolCreateInfo.flags = 0;
				poolCreateInfo.maxSets = XENON_VK_MAX_DESCRIPTOR_SETS_COUNT;
				poolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
				poolCreateInfo.pPoolSizes = poolSizes.data();

				XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkCreateDescriptorPool(m_pDevice->getLogicalDevice(), &poolCreateInfo, nullptr, &pool), "Failed to create the descriptor pool!");
				storage.m_Pools.emplace_back(pool, 1);
			}

			// Allocate the descriptor set.
			VkDescriptorSetAllocateInfo allocateInfo = {};
			allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocateInfo.pNext = nullptr;
			allocateInfo.descriptorPool = pool;
			allocateInfo.descriptorSetCount = 1;
			allocateInfo.pSetLayouts = &storage.m_Layout;

			VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
			XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkAllocateDescriptorSets(m_pDevice->getLogicalDevice(), &allocateInfo, &descriptorSet), "Failed to allocate the descriptor set!");

			return std::make_pair(pool, descriptorSet);
		}

		void VulkanDescriptorSetManager::freeDescriptorSet(VkDescriptorPool pool, VkDescriptorSet descriptorSet, const std::vector<DescriptorBindingInfo>& bindingInfo, DescriptorType descriptorType)
		{
			const auto bindingHash = GenerateHash(reinterpret_cast<const std::byte*>(bindingInfo.data()), bindingInfo.size() * sizeof(DescriptorBindingInfo));
			auto& storage = m_DescriptorSetStorages[EnumToInt(descriptorType)][bindingHash];

			XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkFreeDescriptorSets(m_pDevice->getLogicalDevice(), pool, 1, &descriptorSet), "Failed to free the descriptor set!");

			// Update the pool list.
			for (uint32_t i = 0; i < storage.m_Pools.size(); i++)
			{
				auto& [descriptorPool, count] = storage.m_Pools[i];
				count--;

				// Deallocate the pool and remove it from the list.
				if (count == 0)
				{
					m_pDevice->getDeviceTable().vkDestroyDescriptorPool(m_pDevice->getLogicalDevice(), pool, nullptr);
					storage.m_Pools.erase(storage.m_Pools.begin() + i);
					return;
				}
			}
		}
	}
}