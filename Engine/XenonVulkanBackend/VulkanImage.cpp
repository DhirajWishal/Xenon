// Copyright 2022-2023 Nexonous
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
			{
				usageFlags |= VK_IMAGE_USAGE_STORAGE_BIT;

				m_CurrentLayout = VK_IMAGE_LAYOUT_GENERAL;
			}

			if (specification.m_Usage & ImageUsage::Graphics)
			{
				usageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;

				m_CurrentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			}

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

			case Xenon::Backend::ImageType::ThreeDimensional:
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
			imageCreateInfo.mipLevels = specification.m_EnableMipMaps ? static_cast<uint32_t>(std::floor(std::log2(std::max(specification.m_Width, specification.m_Height)))) + 1 : 1;
			imageCreateInfo.arrayLayers = specification.m_Layers;
			imageCreateInfo.samples = VulkanDevice::ConvertSamplingCount(specification.m_MultiSamplingCount);
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
				imageCreateInfo.format = VulkanDevice::ConvertFormat(candidate);

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

			m_pDevice->getAllocator().access([this, imageCreateInfo, allocationCreateInfo](VmaAllocator allocator)
				{
					XENON_VK_ASSERT(vmaCreateImage(allocator, &imageCreateInfo, &allocationCreateInfo, &m_Image, &m_Allocation, nullptr), "Failed to create the image!");
				}
			);

			m_AttachmentDescription.flags = 0;
			m_AttachmentDescription.format = imageCreateInfo.format;
			m_AttachmentDescription.samples = imageCreateInfo.samples;

			// Change the image layout to the defined layout.
			auto commandBuffers = VulkanCommandRecorder(m_pDevice, CommandRecorderUsage::Transfer);
			commandBuffers.begin();
			commandBuffers.changeImageLayout(m_Image, VK_IMAGE_LAYOUT_UNDEFINED, m_CurrentLayout, getAspectFlags());
			commandBuffers.end();
			commandBuffers.submit();
			commandBuffers.wait();
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
				m_pDevice->getAllocator().access([this](VmaAllocator allocator) { vmaDestroyImage(allocator, m_Image, m_Allocation); });
		}

		void VulkanImage::copyFrom(Buffer* pSrcBuffer, CommandRecorder* pCommandRecorder /*= nullptr*/)
		{
			OPTICK_EVENT();

			if (pCommandRecorder)
			{
				const auto previousLayout = m_CurrentLayout;
				pCommandRecorder->as<VulkanCommandRecorder>()->changeImageLayout(m_Image, m_CurrentLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, getAspectFlags());
				m_CurrentLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

				pCommandRecorder->as<VulkanCommandRecorder>()->copy(pSrcBuffer, 0, this, glm::vec3(getWidth(), getHeight(), 1));

				const auto newLayout = previousLayout == VK_IMAGE_LAYOUT_UNDEFINED ? VK_IMAGE_LAYOUT_GENERAL : previousLayout;
				pCommandRecorder->as<VulkanCommandRecorder>()->changeImageLayout(m_Image, m_CurrentLayout, newLayout, getAspectFlags());
				m_CurrentLayout = newLayout;
			}
			else
			{
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
		}

		void VulkanImage::copyFrom(Image* pSrcImage, CommandRecorder* pCommandRecorder /*= nullptr*/)
		{
			OPTICK_EVENT();

			if (pCommandRecorder)
			{
				pCommandRecorder->copy(pSrcImage, glm::vec3(0), this, glm::vec3(0));
			}
			else
			{
				auto commandBuffers = VulkanCommandRecorder(m_pDevice, CommandRecorderUsage::Transfer);
				commandBuffers.begin();

				commandBuffers.copy(pSrcImage, glm::vec3(0), this, glm::vec3(0));

				commandBuffers.end();
				commandBuffers.submit();
				commandBuffers.wait();
			}
		}

		void VulkanImage::generateMipMaps(CommandRecorder* pCommandRecorder /*= nullptr*/)
		{
			if (pCommandRecorder)
			{
				blitImage(pCommandRecorder->as<VulkanCommandRecorder>());
			}
			else
			{
				auto commandRecorder = VulkanCommandRecorder(m_pDevice, CommandRecorderUsage::Transfer);
				commandRecorder.begin();
				commandRecorder.end();
				blitImage(&commandRecorder);
				commandRecorder.submit();
				commandRecorder.wait();
			}
		}

		VkImageAspectFlags VulkanImage::getAspectFlags() const
		{
			return m_Specification.m_Usage & ImageUsage::DepthAttachment ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		}

		void VulkanImage::blitImage(VulkanCommandRecorder* pCommandRecorder)
		{
			// Check if image format supports linear blitting
			VkFormatProperties formatProperties = {};
			vkGetPhysicalDeviceFormatProperties(m_pDevice->getPhysicalDevice(), VulkanDevice::ConvertFormat(getDataFormat()), &formatProperties);

			if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
			{
				XENON_LOG_ERROR("The current image format does not support mip-map generation!");
				return;
			}

			VkImageMemoryBarrier barrier = {};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.image = m_Image;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.subresourceRange.aspectMask = getAspectFlags();
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = 1;
			barrier.subresourceRange.levelCount = 1;
			barrier.oldLayout = m_CurrentLayout;

			int32_t mipWidth = getWidth();
			int32_t mipHeight = getHeight();
			uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(mipWidth, mipHeight)))) + 1;

			VkCommandBuffer commandBuffer = *pCommandRecorder->getCurrentCommandBuffer();
			for (uint32_t i = 1; i < mipLevels; i++)
			{
				barrier.subresourceRange.baseMipLevel = i - 1;
				barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

				m_pDevice->getDeviceTable().vkCmdPipelineBarrier(commandBuffer,
					VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
					0, nullptr,
					0, nullptr,
					1, &barrier);

				VkImageBlit blit = {};
				blit.srcOffsets[0] = { 0, 0, 0 };
				blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
				blit.srcSubresource.aspectMask = barrier.subresourceRange.aspectMask;
				blit.srcSubresource.mipLevel = i - 1;
				blit.srcSubresource.baseArrayLayer = 0;
				blit.srcSubresource.layerCount = 1;
				blit.dstOffsets[0] = { 0, 0, 0 };
				blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
				blit.dstSubresource.aspectMask = barrier.subresourceRange.aspectMask;
				blit.dstSubresource.mipLevel = i;
				blit.dstSubresource.baseArrayLayer = 0;
				blit.dstSubresource.layerCount = 1;

				m_pDevice->getDeviceTable().vkCmdBlitImage(commandBuffer,
					m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					1, &blit,
					VK_FILTER_LINEAR);

				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				m_pDevice->getDeviceTable().vkCmdPipelineBarrier(commandBuffer,
					VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
					0, nullptr,
					0, nullptr,
					1, &barrier);

				if (mipWidth > 1) mipWidth /= 2;
				if (mipHeight > 1) mipHeight /= 2;

				barrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			}

			barrier.subresourceRange.baseMipLevel = mipLevels - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			m_pDevice->getDeviceTable().vkCmdPipelineBarrier(commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			m_CurrentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
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