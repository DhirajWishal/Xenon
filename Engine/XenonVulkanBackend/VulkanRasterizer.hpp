// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/Rasterizer.hpp"

#include "VulkanImage.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan rasterizer class.
		 */
		class VulkanRasterizer final : public Rasterizer, public VulkanDeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param width The width of the render target.
			 * @param height The height of the render target.
			 * @param attachmentTypes The attachment types the render target should support.
			 * @param enableTripleBuffering Whether to enable triple-buffering. Default is false.
			 * @param multiSampleCount Multi-sampling count to use. Default is x1.
			 */
			explicit VulkanRasterizer(VulkanDevice* pDevice, uint32_t width, uint32_t height, AttachmentType attachmentTypes, bool enableTripleBuffering = false, MultiSamplingCount multiSampleCount = MultiSamplingCount::x1);

			/**
			 * Destructor.
			 */
			~VulkanRasterizer() override;

			/**
			 * Get the image attachment of the relevant attachment type.
			 *
			 * @param type The attachment type.
			 * @return The attachment image.
			 */
			XENON_NODISCARD Image* getImageAttachment(AttachmentType type) override;

			/**
			 * Get the render pass.
			 *
			 * @return The render pass.
			 */
			XENON_NODISCARD VkRenderPass getRenderPass() const { return m_RenderPass; }

			/**
			 * Get the framebuffer.
			 *
			 * @return The framebuffer.
			 */
			XENON_NODISCARD VkFramebuffer getFramebuffer() const { return m_Framebuffers[m_FrameIndex]; }

		private:
			/**
			 * Setup the image attachments.
			 */
			void setupAttachments();

			/**
			 * Create the render pass.
			 */
			void createRenderPass();

			/**
			 * Create the framebuffers.
			 */
			void createFramebuffers();

			/**
			 * Create an image view using the created image, its image aspect flags and the format.
			 *
			 * @param image The image to create the view for.
			 * @param aspectFlags The image aspect flags.
			 * @param format The image format.
			 */
			void createImageView(VkImage image, VkImageAspectFlags aspectFlags, VkFormat format);

		private:
			VkRenderPass m_RenderPass = VK_NULL_HANDLE;
			std::vector<VkImageView> m_AttachmentViews;
			std::vector<VkFramebuffer> m_Framebuffers;
			std::vector<VulkanImage> m_ImageAttachments;
		};
	}
}