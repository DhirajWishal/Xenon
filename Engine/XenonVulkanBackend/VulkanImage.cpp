// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanImage.hpp"
#include "VulkanMacros.hpp"

namespace Xenon
{
	namespace Backend
	{
		VulkanImage::VulkanImage(VulkanDevice* pDevice, const ImageSpecification& specification)
			: VulkanDeviceBoundObject(pDevice)
			, Image(pDevice, specification)
		{
			// Resolve the image usage.
			VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			if (specification.m_Usage & ImageUsage::Storage)
				usageFlags |= VK_IMAGE_USAGE_STORAGE_BIT;

			if (specification.m_Usage & ImageUsage::Graphics)
				usageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;

			if (specification.m_Usage & ImageUsage::ColorAttachment)
				usageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

			if (specification.m_Usage & ImageUsage::DepthAttachment)
				usageFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

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
			imageCreateInfo.format = m_pDevice->convertFormat(specification.m_Format);
			imageCreateInfo.extent.width = specification.m_Width;
			imageCreateInfo.extent.height = specification.m_Height;
			imageCreateInfo.extent.depth = specification.m_Depth;
			imageCreateInfo.mipLevels = 1;	// TODO: Come up with a better system.
			imageCreateInfo.arrayLayers = specification.m_Layers;
			imageCreateInfo.samples = m_pDevice->convertSamplingCount(specification.m_MultiSamplingCount);
			imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageCreateInfo.usage = usageFlags;
			imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageCreateInfo.queueFamilyIndexCount = 0;
			imageCreateInfo.pQueueFamilyIndices = nullptr;
			imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

			VmaAllocationCreateInfo allocationCreateInfo = {};
			allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

			XENON_VK_ASSERT(vmaCreateImage(m_pDevice->getAllocator(), &imageCreateInfo, &allocationCreateInfo, &m_Image, &m_Allocation, nullptr), "Failed to create the image!");
		}

		VulkanImage::VulkanImage(VulkanImage&& other) noexcept
			: VulkanDeviceBoundObject(std::exchange(other.m_pDevice, nullptr))
			, Image(static_cast<Image&&>(other))
			, m_Image(std::exchange(other.m_Image, VK_NULL_HANDLE))
			, m_Allocation(std::exchange(other.m_Allocation, nullptr))
		{
		}

		VulkanImage::~VulkanImage()
		{
			if (m_pDevice)
			{
				try
				{
					m_pDevice->getInstance()->getDeletionQueue().insert([pDevice = m_pDevice, image = m_Image, allocation = m_Allocation]
						{
							vmaDestroyImage(pDevice->getAllocator(), image, allocation);
						}
					);
				}
				catch (...)
				{
					XENON_VK_ASSERT(VK_ERROR_UNKNOWN, "Failed to push the image deletion function to the deletion queue!");
				}
			}
		}

		Xenon::Backend::VulkanImage& VulkanImage::operator=(VulkanImage&& other) noexcept
		{
			Image::operator=(std::move(other));
			m_pDevice = std::exchange(other.m_pDevice, nullptr);
			m_Image = std::exchange(other.m_Image, VK_NULL_HANDLE);
			m_Allocation = std::exchange(other.m_Allocation, nullptr);

			return *this;
		}
	}
}