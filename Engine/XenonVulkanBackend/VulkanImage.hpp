// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/Image.hpp"

#include "VulkanDeviceBoundObject.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan image class.
		 */
		class VulkanImage final : public Image, public VulkanDeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param specification The image specification.
			 */
			explicit VulkanImage(VulkanDevice* pDevice, const ImageSpecification& specification);

			/**
			 * Move constructor.
			 *
			 * @param other The other image.
			 */
			VulkanImage(VulkanImage&& other) noexcept;

			/**
			 * Destructor.
			 */
			~VulkanImage() override;

			/**
			 * Copy image data from a source buffer.
			 *
			 * @param pSrcBuffer The source buffer pointer.
			 */
			void copyFrom(Buffer* pSrcBuffer) override;

			/**
			 * Get the Vulkan image handle.
			 *
			 * @return The image.
			 */
			[[nodiscard]] VkImage getImage() const { return m_Image; }

			/**
			 * Get the image aspect flags.
			 *
			 * @return The aspect flags.
			 */
			[[nodiscard]] VkImageAspectFlags getAspectFlags() const;

			/**
			 * Get the image's attachment description.
			 *
			 * @return The Vulkan attachment description.
			 */
			[[nodiscard]] VkAttachmentDescription getAttachmentDescription() const { return m_AttachmentDescription; }

			/**
			 * Get the current image layout.
			 *
			 * @return The image layout.
			 */
			[[nodiscard]] VkImageLayout getImageLayout() const { return m_CurrentLayout; }

			/**
			 * Set the current image layout.
			 *
			 * @param layout The layout to set.
			 */
			void setImageLayout(VkImageLayout layout) { m_CurrentLayout = layout; }

		public:
			/**
			 * Move assignment operator.
			 *
			 * @param other The other image.
			 * @return The move-assigned image.
			 */
			VulkanImage& operator=(VulkanImage&& other) noexcept;

		private:
			VkAttachmentDescription m_AttachmentDescription = {};

			VkImage m_Image = VK_NULL_HANDLE;
			VmaAllocation m_Allocation = nullptr;

			VkImageLayout m_CurrentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		};
	}
}