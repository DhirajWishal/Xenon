// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanRasterizer.hpp"
#include "VulkanMacros.hpp"

namespace Xenon
{
	namespace Backend
	{
		VulkanRasterizer::VulkanRasterizer(VulkanDevice* pDevice, Camera* pCamera, AttachmentType attachmentTypes, bool enableTripleBuffering /*= false*/, MultiSamplingCount multiSampleCount /*= MultiSamplingCount::x1*/)
			: Rasterizer(pDevice, pCamera, attachmentTypes, enableTripleBuffering, multiSampleCount)
			, VulkanDeviceBoundObject(pDevice)
		{
			// Setup the image attachments.
			setupAttachments();

			// Create the render pass.
			createRenderPass();

			// Create the frame buffers.
			createFramebuffers();
		}

		VulkanRasterizer::~VulkanRasterizer()
		{
			for (const auto& view : m_AttachmentViews)
				m_pDevice->getDeviceTable().vkDestroyImageView(m_pDevice->getLogicalDevice(), view, nullptr);

			m_ImageAttachments.clear();

			for (const auto& framebuffer : m_Framebuffers)
				m_pDevice->getDeviceTable().vkDestroyFramebuffer(m_pDevice->getLogicalDevice(), framebuffer, nullptr);

			m_pDevice->getDeviceTable().vkDestroyRenderPass(m_pDevice->getLogicalDevice(), m_RenderPass, nullptr);
		}

		Xenon::Backend::Image* VulkanRasterizer::getImageAttachment(AttachmentType type)
		{
			if (m_AttachmentTypes & type)
			{
				uint8_t index = 0;
				if (m_AttachmentTypes & AttachmentType::Color && type != AttachmentType::Color)
					index++;

				if (m_AttachmentTypes & AttachmentType::EntityID && type != AttachmentType::EntityID)
					index++;

				if (m_AttachmentTypes & AttachmentType::Normal && type != AttachmentType::Normal)
					index++;

				if (m_AttachmentTypes & AttachmentType::Depth && type != AttachmentType::Depth)
					index++;

				if (m_AttachmentTypes & AttachmentType::Stencil && type != AttachmentType::Stencil)
					index++;

				// Return the attachment pointer.
				return &m_ImageAttachments[index];
			}

			XENON_LOG_FATAL("The requested attachment type is not present in the rasterizer!");
			return nullptr;
		}

		void VulkanRasterizer::setupAttachments()
		{
			ImageSpecification specification;
			specification.m_Width = m_pCamera->getWidth();
			specification.m_Height = m_pCamera->getHeight();
			specification.m_Depth = 1;
			specification.m_EnableMipMaps = false;

			// The few attachments below are color attachments.
			specification.m_Usage = ImageUsage::ColorAttachment | ImageUsage::Storage;

			// Create and add the color attachment if required.
			if (m_AttachmentTypes & AttachmentType::Color)
			{
				specification.m_Format = DataFormat::R8G8B8A8_UNORMAL | DataFormat::R8G8B8A8_SRGB;
				specification.m_MultiSamplingCount = m_MultiSamplingCount;

				const auto& image = m_ImageAttachments.emplace_back(m_pDevice, specification);
				createImageView(image.getImage(), VK_IMAGE_ASPECT_COLOR_BIT, m_pDevice->convertFormat(image.getDataFormat()));
			}

			// The rest of the attachments don't need multi-sampling.
			specification.m_MultiSamplingCount = MultiSamplingCount::x1;

			// Create and add the entity ID attachment if required.
			if (m_AttachmentTypes & AttachmentType::EntityID)
			{
				specification.m_Format = DataFormat::R32_SFLOAT;

				const auto& image = m_ImageAttachments.emplace_back(m_pDevice, specification);
				createImageView(image.getImage(), image.getAspectFlags(), m_pDevice->convertFormat(image.getDataFormat()));
			}

			// Create and add the normal attachment if required.
			if (m_AttachmentTypes & AttachmentType::Normal)
			{
				specification.m_Format = DataFormat::R32G32B32_SFLOAT;

				const auto& image = m_ImageAttachments.emplace_back(m_pDevice, specification);
				createImageView(image.getImage(), image.getAspectFlags(), m_pDevice->convertFormat(image.getDataFormat()));
			}

			// Create and add the depth attachment with stencil attachment if required.
			if (m_AttachmentTypes & AttachmentType::Depth && m_AttachmentTypes & AttachmentType::Stencil)
			{
				specification.m_Usage = ImageUsage::DepthAttachment;
				specification.m_Format = DataFormat::D32_SFLOAT_S8_UINT | DataFormat::D24_UNORMAL_S8_UINT | DataFormat::D16_UNORMAL_S8_UINT;

				const auto& image = m_ImageAttachments.emplace_back(m_pDevice, specification);
				createImageView(image.getImage(), image.getAspectFlags(), m_pDevice->convertFormat(image.getDataFormat()));
			}

			// Create and add the depth attachment if required.
			else if (m_AttachmentTypes & AttachmentType::Depth)
			{
				specification.m_Usage = ImageUsage::DepthAttachment | ImageUsage::Storage;
				specification.m_Format = DataFormat::D32_SFLOAT | DataFormat::D16_SINT;

				const auto& image = m_ImageAttachments.emplace_back(m_pDevice, specification);
				createImageView(image.getImage(), image.getAspectFlags(), m_pDevice->convertFormat(image.getDataFormat()));
			}

			// Create and add the stencil attachment if required.
			else if (m_AttachmentTypes & AttachmentType::Stencil)
			{
				specification.m_Usage = ImageUsage::DepthAttachment;
				specification.m_Format = DataFormat::S8_UINT;

				const auto& image = m_ImageAttachments.emplace_back(m_pDevice, specification);
				createImageView(image.getImage(), image.getAspectFlags(), m_pDevice->convertFormat(image.getDataFormat()));
			}
		}

		void VulkanRasterizer::createRenderPass()
		{
			// Create attachment descriptions and references.
			std::vector<VkAttachmentReference> colorAttachments;
			std::vector<VkAttachmentReference> depthAttachments;
			std::vector<VkAttachmentDescription> attachmentDescriptions;

			VkAttachmentReference attachmentReference = {};
			for (const auto& attachment : m_ImageAttachments)
			{
				attachmentDescriptions.emplace_back(attachment.getAttachmentDescription());

				// Setup the attachment references.
				if (attachment.getUsage() & ImageUsage::ColorAttachment)
				{
					attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					colorAttachments.emplace_back(attachmentReference);
				}

				else if (attachment.getUsage() & ImageUsage::DepthAttachment)
				{
					attachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
					depthAttachments.emplace_back(attachmentReference);
				}

				attachmentReference.attachment++;
			}

			// Create the subpass dependencies.
			std::array<VkSubpassDependency, 2> subpassDependencies = {};
			subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			subpassDependencies[0].dstSubpass = 0;
			subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subpassDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			subpassDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			subpassDependencies[1].srcSubpass = 0;
			subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
			subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			subpassDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			subpassDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			subpassDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			// Create the subpass description.
			VkSubpassDescription subpassDescription = {};
			subpassDescription.flags = 0;
			subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpassDescription.inputAttachmentCount = 0;
			subpassDescription.pInputAttachments = nullptr;
			subpassDescription.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
			subpassDescription.pColorAttachments = colorAttachments.data();
			subpassDescription.pResolveAttachments = nullptr;
			subpassDescription.pDepthStencilAttachment = depthAttachments.data();
			subpassDescription.preserveAttachmentCount = 0;
			subpassDescription.pPreserveAttachments = nullptr;

			// Create the render pass.
			VkRenderPassCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
			createInfo.pAttachments = attachmentDescriptions.data();
			createInfo.subpassCount = 1;
			createInfo.pSubpasses = &subpassDescription;
			createInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
			createInfo.pDependencies = subpassDependencies.data();

			XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkCreateRenderPass(m_pDevice->getLogicalDevice(), &createInfo, nullptr, &m_RenderPass), "Failed to create the render pass!");
		}

		void VulkanRasterizer::createFramebuffers()
		{
			VkFramebufferCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.renderPass = m_RenderPass;
			createInfo.attachmentCount = static_cast<uint32_t>(m_AttachmentViews.size());
			createInfo.pAttachments = m_AttachmentViews.data();
			createInfo.width = m_pCamera->getWidth();
			createInfo.height = m_pCamera->getHeight();
			createInfo.layers = 1;

			const auto bufferCount = m_bEnableTripleBuffering ? 3 : 1;
			for (uint32_t i = 0; i < bufferCount; i++)
			{
				auto& framebuffer = m_Framebuffers.emplace_back();
				XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkCreateFramebuffer(m_pDevice->getLogicalDevice(), &createInfo, nullptr, &framebuffer), "Failed to create the frame buffer!");
			}
		}

		void VulkanRasterizer::createImageView(VkImage image, VkImageAspectFlags aspectFlags, VkFormat format)
		{
			VkImageViewCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.image = image;
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = format;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = aspectFlags;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			auto& imageView = m_AttachmentViews.emplace_back();
			XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkCreateImageView(m_pDevice->getLogicalDevice(), &createInfo, nullptr, &imageView), "Failed to create the image view!");
		}
	}
}