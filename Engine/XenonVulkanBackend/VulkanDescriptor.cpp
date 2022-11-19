// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanDescriptor.hpp"
#include "VulkanMacros.hpp"

#include "VulkanBuffer.hpp"
#include "VulkanImage.hpp"

namespace Xenon
{
	namespace Backend
	{
		VulkanDescriptor::VulkanDescriptor(VulkanDevice* pDevice, const std::vector<DescriptorBindingInfo>& bindingInfo, DescriptorType type)
			: Descriptor(pDevice, bindingInfo, type)
			, VulkanDeviceBoundObject(pDevice)
		{
			// Create a new descriptor set.
			const auto [pool, set] = pDevice->createDescriptorSet(bindingInfo);

			m_Pool = pool;
			m_DescriptorSet = set;
		}

		VulkanDescriptor::~VulkanDescriptor()
		{
			try
			{
				m_pDevice->getInstance()->getDeletionQueue().insert([pDevice = m_pDevice, pool = m_Pool, set = m_DescriptorSet, bindingInfo = std::move(m_BindingInformation)]
					{
						pDevice->freeDescriptorSet(pool, set, bindingInfo);
					}
				);
			}
			catch (...)
			{
				XENON_VK_ASSERT(VK_ERROR_UNKNOWN, "Failed to push the descriptor deletion function to the deletion queue!");
			}
		}

		void VulkanDescriptor::attach(uint32_t binding, Buffer* pBuffer)
		{
			auto pVkBuffer = pBuffer->as<VulkanBuffer>();
			const auto& bufferInfo = pVkBuffer->getDescriptorBufferInfo();

			VkWriteDescriptorSet writeDescriptorSet = {};
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.pNext = nullptr;
			writeDescriptorSet.dstSet = m_DescriptorSet;
			writeDescriptorSet.dstBinding = binding;
			writeDescriptorSet.dstArrayElement = 0;
			writeDescriptorSet.descriptorCount = 1;
			writeDescriptorSet.descriptorType = m_pDevice->convertResourceType(m_BindingInformation[binding].m_Type);
			writeDescriptorSet.pImageInfo = nullptr;
			writeDescriptorSet.pBufferInfo = &bufferInfo;
			writeDescriptorSet.pTexelBufferView = nullptr;

			m_pDevice->getDeviceTable().vkUpdateDescriptorSets(m_pDevice->getLogicalDevice(), 1, &writeDescriptorSet, 0, nullptr);
		}

		void VulkanDescriptor::attach(uint32_t binding, Image* pImage, ImageView* pView, ImageSampler* pSampler, ImageUsage usage)
		{
			auto pVkImage = pImage->as<VulkanImage>();

			// TODO
			VkDescriptorImageInfo imageInfo = {};
			imageInfo.sampler = VK_NULL_HANDLE;
			imageInfo.imageView = VK_NULL_HANDLE;
			imageInfo.imageLayout = pVkImage->getImageLayout();	// FIXME

			VkWriteDescriptorSet writeDescriptorSet = {};
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.pNext = nullptr;
			writeDescriptorSet.dstSet = m_DescriptorSet;
			writeDescriptorSet.dstBinding = binding;
			writeDescriptorSet.dstArrayElement = 0;
			writeDescriptorSet.descriptorCount = 1;
			writeDescriptorSet.descriptorType = m_pDevice->convertResourceType(m_BindingInformation[binding].m_Type);
			writeDescriptorSet.pImageInfo = &imageInfo;
			writeDescriptorSet.pBufferInfo = nullptr;
			writeDescriptorSet.pTexelBufferView = nullptr;

			m_pDevice->getDeviceTable().vkUpdateDescriptorSets(m_pDevice->getLogicalDevice(), 1, &writeDescriptorSet, 0, nullptr);
		}
	}
}