// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanDescriptor.hpp"
#include "VulkanMacros.hpp"
#include "VulkanDescriptorSetManager.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanImageView.hpp"
#include "VulkanImageSampler.hpp"

#include <optick.h>

namespace Xenon
{
	namespace Backend
	{
		VulkanDescriptor::VulkanDescriptor(VulkanDevice* pDevice, const std::vector<DescriptorBindingInfo>& bindingInfo, DescriptorType type)
			: Descriptor(pDevice, bindingInfo, type)
			, VulkanDeviceBoundObject(pDevice)
		{
			// Create a new descriptor set.
			const auto [pool, set] = pDevice->getDescriptorSetManager()->createDescriptorSet(bindingInfo);

			m_Pool = pool;
			m_DescriptorSet = set;
		}

		VulkanDescriptor::~VulkanDescriptor()
		{
			m_pDevice->getDescriptorSetManager()->freeDescriptorSet(m_Pool, m_DescriptorSet, m_BindingInformation);
		}

		void VulkanDescriptor::attach(uint32_t binding, Buffer* pBuffer)
		{
			OPTICK_EVENT();

			const auto& bufferInfo = pBuffer->as<VulkanBuffer>()->getDescriptorBufferInfo();

			VkWriteDescriptorSet writeDescriptorSet = {};
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.pNext = nullptr;
			writeDescriptorSet.dstSet = m_DescriptorSet;
			writeDescriptorSet.dstBinding = binding;
			writeDescriptorSet.dstArrayElement = 0;
			writeDescriptorSet.descriptorCount = 1;
			writeDescriptorSet.descriptorType = VulkanDevice::ConvertResourceType(m_BindingInformation[binding].m_Type);
			writeDescriptorSet.pImageInfo = nullptr;
			writeDescriptorSet.pBufferInfo = &bufferInfo;
			writeDescriptorSet.pTexelBufferView = nullptr;

			m_pDevice->getDeviceTable().vkUpdateDescriptorSets(m_pDevice->getLogicalDevice(), 1, &writeDescriptorSet, 0, nullptr);
		}

		void VulkanDescriptor::attach(uint32_t binding, Image* pImage, ImageView* pView, ImageSampler* pSampler, ImageUsage usage)
		{
			OPTICK_EVENT();

			VkDescriptorImageInfo imageInfo = {};
			imageInfo.sampler = pSampler->as<VulkanImageSampler>()->getSampler();
			imageInfo.imageView = pView->as<VulkanImageView>()->getView();
			imageInfo.imageLayout = pImage->as<VulkanImage>()->getImageLayout();

			VkWriteDescriptorSet writeDescriptorSet = {};
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.pNext = nullptr;
			writeDescriptorSet.dstSet = m_DescriptorSet;
			writeDescriptorSet.dstBinding = binding;
			writeDescriptorSet.dstArrayElement = 0;
			writeDescriptorSet.descriptorCount = 1;
			writeDescriptorSet.descriptorType = VulkanDevice::ConvertResourceType(m_BindingInformation[binding].m_Type);
			writeDescriptorSet.pImageInfo = &imageInfo;
			writeDescriptorSet.pBufferInfo = nullptr;
			writeDescriptorSet.pTexelBufferView = nullptr;

			if (writeDescriptorSet.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
			{
				constexpr std::array<VkImageLayout, 9> allowedLayouts = {
					VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					VK_IMAGE_LAYOUT_GENERAL,
					VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL,
					VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL,
					VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
					VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
					VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL,
					VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL
				};

				bool isAllowed = false;
				for (const auto layout : allowedLayouts)
				{
					if (layout == imageInfo.imageLayout)
					{
						isAllowed = true;
						break;
					}
				}

				if (!isAllowed)
				{
					// Inform the command recorder to change the image layout to the required one.
					// Inform the command recorder to change the image layout to the previous one.
				}
			}
			else if (writeDescriptorSet.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
			{
				constexpr std::array<VkImageLayout, 10> allowedLayouts = {
					VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					VK_IMAGE_LAYOUT_GENERAL,
					VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR,
					VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL,
					VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL,
					VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL,
					VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL,
					VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL_KHR,
					VK_IMAGE_LAYOUT_ATTACHMENT_FEEDBACK_LOOP_OPTIMAL_EXT
				};

				bool isAllowed = false;
				for (const auto layout : allowedLayouts)
				{
					if (layout == imageInfo.imageLayout)
					{
						isAllowed = true;
						break;
					}
				}

				if (!isAllowed)
				{
					// Inform the command recorder to change the image layout to the required one.
					// Inform the command recorder to change the image layout to the previous one.
				}
			}
			else if (writeDescriptorSet.descriptorType == VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
			{
				constexpr std::array<VkImageLayout, 8> allowedLayouts = {
					VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					VK_IMAGE_LAYOUT_GENERAL,
					VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR,
					VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL,
					VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL,
					VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL_KHR,
					VK_IMAGE_LAYOUT_ATTACHMENT_FEEDBACK_LOOP_OPTIMAL_EXT
				};

				bool isAllowed = false;
				for (const auto layout : allowedLayouts)
				{
					if (layout == imageInfo.imageLayout)
					{
						isAllowed = true;
						break;
					}
				}

				if (!isAllowed)
				{
					// Inform the command recorder to change the image layout to the required one.
					// Inform the command recorder to change the image layout to the previous one.
				}
			}

			m_pDevice->getDeviceTable().vkUpdateDescriptorSets(m_pDevice->getLogicalDevice(), 1, &writeDescriptorSet, 0, nullptr);
		}
	}
}