// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanImage.hpp"
#include "VulkanMacros.hpp"
#include "VulkanCommandRecorder.hpp"
#include "VulkanBuffer.hpp"

#include <optick.h>

namespace Xenon
{
	namespace Backend
	{
		VulkanImage::VulkanImage(VulkanDevice* pDevice, const ImageSpecification& specification)
			: Image(pDevice, specification)
			, VulkanDeviceBoundObject(pDevice)
		{
			// Resolve the image usage.
			VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			if (specification.m_Usage & ImageUsage::Storage)
				usageFlags |= VK_IMAGE_USAGE_STORAGE_BIT;

			if (specification.m_Usage & ImageUsage::Graphics)
				usageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;

			if (specification.m_Usage & ImageUsage::ColorAttachment)
			{
				usageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
				usageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;

				m_AttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				m_AttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				m_AttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				m_AttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				m_AttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				m_AttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				m_CurrentLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}

			else if (specification.m_Usage & ImageUsage::DepthAttachment)
			{
				usageFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
				usageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;

				m_AttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				m_AttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				m_AttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				m_AttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				m_AttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				m_AttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
				m_CurrentLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			}

			// Resolve the image type.
			VkImageType type = VK_IMAGE_TYPE_2D;
			switch (specification.m_Type)
			{
			case Xenon::Backend::ImageType::OneDimensional:
				type = VK_IMAGE_TYPE_1D;
				break;

			case Xenon::Backend::ImageType::TwoDimensional:
			case Xenon::Backend::ImageType::CubeMap:
				break;

			case Xenon::Backend::ImageType::ThreeDImentional:
				type = VK_IMAGE_TYPE_3D;
				break;

			default:
				XENON_LOG_ERROR("Invalid image type! Defaulting to TwoDimensional.");
				break;
			}

			// Create the image.
			VkImageCreateInfo imageCreateInfo = {};
			imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageCreateInfo.pNext = nullptr;
			imageCreateInfo.flags = specification.m_Type == ImageType::CubeMap ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
			imageCreateInfo.imageType = type;
			imageCreateInfo.extent.width = specification.m_Width;
			imageCreateInfo.extent.height = specification.m_Height;
			imageCreateInfo.extent.depth = specification.m_Depth;
			imageCreateInfo.mipLevels = /*specification.m_EnableMipMaps*/ 1;
			imageCreateInfo.arrayLayers = specification.m_Layers;
			imageCreateInfo.samples = m_pDevice->convertSamplingCount(specification.m_MultiSamplingCount);
			imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageCreateInfo.usage = usageFlags;
			imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageCreateInfo.queueFamilyIndexCount = 0;
			imageCreateInfo.pQueueFamilyIndices = nullptr;
			imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

			// Resolve the image format.
			bool formatFound = false;
			for (const auto candidates = GetCandidateFormats(specification.m_Format); const auto candidate : candidates)
			{
				imageCreateInfo.format = m_pDevice->convertFormat(candidate);

				// Get the format properties.
				VkImageFormatProperties formatProperties = {};
				const auto result = vkGetPhysicalDeviceImageFormatProperties(
					m_pDevice->getPhysicalDevice(),
					imageCreateInfo.format,
					imageCreateInfo.imageType,
					imageCreateInfo.tiling,
					imageCreateInfo.usage,
					imageCreateInfo.flags,
					&formatProperties
				);

				// If the format is supported, we can go with it.
				if (result == VK_SUCCESS)
				{
					m_Specification.m_Format = candidate;
					formatFound = true;
					break;
				}
			}

			// Check if we found a format.
			if (!formatFound)
			{
				XENON_LOG_FATAL("The provided format (with or without candidates) cannot be used to create the image!");
				return;
			}

			VmaAllocationCreateInfo allocationCreateInfo = {};
			allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

			XENON_VK_ASSERT(vmaCreateImage(m_pDevice->getAllocator(), &imageCreateInfo, &allocationCreateInfo, &m_Image, &m_Allocation, nullptr), "Failed to create the image!");

			m_AttachmentDescription.flags = 0;
			m_AttachmentDescription.format = imageCreateInfo.format;
			m_AttachmentDescription.samples = imageCreateInfo.samples;
		}

		VulkanImage::VulkanImage(VulkanImage&& other) noexcept
			: Image(std::move(other))
			, VulkanDeviceBoundObject(std::move(other))
			, m_AttachmentDescription(std::exchange(other.m_AttachmentDescription, {}))
			, m_Image(std::exchange(other.m_Image, VK_NULL_HANDLE))
			, m_Allocation(std::exchange(other.m_Allocation, nullptr))
			, m_CurrentLayout(std::exchange(other.m_CurrentLayout, VK_IMAGE_LAYOUT_UNDEFINED))
		{
		}

		VulkanImage::~VulkanImage()
		{
			if (m_pDevice)
				vmaDestroyImage(m_pDevice->getAllocator(), m_Image, m_Allocation);
		}

		void VulkanImage::copyFrom(Buffer* pSrcBuffer, CommandRecorder* pCommandRecorder /*= nullptr*/)
		{
			OPTICK_EVENT();

			auto commandBuffers = VulkanCommandRecorder(m_pDevice, CommandRecorderUsage::Transfer);
			commandBuffers.begin();

			const auto previousLayout = m_CurrentLayout;
			commandBuffers.changeImageLayout(m_Image, m_CurrentLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, getAspectFlags());
			m_CurrentLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

			commandBuffers.copy(pSrcBuffer, 0, this, glm::vec3(getWidth(), getHeight(), 1));

			const auto newLayout = previousLayout == VK_IMAGE_LAYOUT_UNDEFINED ? VK_IMAGE_LAYOUT_GENERAL : previousLayout;
			commandBuffers.changeImageLayout(m_Image, m_CurrentLayout, newLayout, getAspectFlags());
			m_CurrentLayout = newLayout;

			commandBuffers.end();
			commandBuffers.submit();
			commandBuffers.wait();
		}

		void VulkanImage::copyFrom(Image* pSrcImage, CommandRecorder* pCommandRecorder /*= nullptr*/)
		{
			auto pVkImage = pSrcImage->as<VulkanImage>();

			auto commandBuffers = VulkanCommandRecorder(m_pDevice, CommandRecorderUsage::Transfer);
			commandBuffers.begin();

			commandBuffers.changeImageLayout(m_Image, m_CurrentLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, getAspectFlags());
			commandBuffers.changeImageLayout(pVkImage->getImage(), pVkImage->getImageLayout(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, pVkImage->getAspectFlags());

			commandBuffers.copy(pSrcImage, glm::vec3(0), this, glm::vec3(0));

			m_CurrentLayout = m_CurrentLayout == VK_IMAGE_LAYOUT_UNDEFINED ? VK_IMAGE_LAYOUT_GENERAL : m_CurrentLayout;
			commandBuffers.changeImageLayout(m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_CurrentLayout, getAspectFlags());
			commandBuffers.changeImageLayout(pVkImage->getImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, pVkImage->getImageLayout(), pVkImage->getAspectFlags());

			commandBuffers.end();
			commandBuffers.submit();
			commandBuffers.wait();
		}

		VkImageAspectFlags VulkanImage::getAspectFlags() const
		{
			return m_Specification.m_Usage & ImageUsage::DepthAttachment ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		}

		Xenon::Backend::VulkanImage& VulkanImage::operator=(VulkanImage&& other) noexcept
		{
			Image::operator=(std::move(other));
			VulkanDeviceBoundObject::operator=(std::move(other));
			m_AttachmentDescription = std::exchange(other.m_AttachmentDescription, {});
			m_Image = std::exchange(other.m_Image, VK_NULL_HANDLE);
			m_Allocation = std::exchange(other.m_Allocation, nullptr);
			m_CurrentLayout = std::exchange(other.m_CurrentLayout, VK_IMAGE_LAYOUT_UNDEFINED);

			return *this;
		}
	}
}