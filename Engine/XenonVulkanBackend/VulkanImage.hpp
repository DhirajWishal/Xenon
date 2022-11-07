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
		class VulkanImage final : public VulkanDeviceBoundObject, public Image
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
		};
	}
}