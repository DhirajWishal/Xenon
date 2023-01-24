// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/Image.hpp"

#include "VulkanDeviceBoundObject.hpp"

namespace Xenon
{
	namespace Backend
	{
		class VulkanCommandRecorder;

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
			 * @param pCommandRecorder The command recorder pointer to record the commands to. Default is nullptr.
			 */
			void copyFrom(Buffer* pSrcBuffer, CommandRecorder* pCommandRecorder = nullptr) override;

			/**
			 * Copy image data from a source image.
			 *
			 * @param pSrcImage The source image.
			 * @param pCommandRecorder The command recorder pointer to record the commands to. Default is nullptr.
			 */
			void copyFrom(Image* pSrcImage, CommandRecorder* pCommandRecorder = nullptr) override;

			/**
			 * Generate mip maps for the currently stored image.
			 *
			 * @param pCommandRecorder The command recorder pointer to record the commands to. Default is nullptr (in which case the backend will create one for this purpose).
			 */
			void generateMipMaps(CommandRecorder* pCommandRecorder = nullptr) override;

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
			/**
			 * Blit image and generate the mip maps.
			 *
			 * @param pCommandRecorder The command recorder to record the commands to.
			 */
			void blitImage(VulkanCommandRecorder* pCommandRecorder);

		private:
			VkAttachmentDescription m_AttachmentDescription = {};

			VkImage m_Image = VK_NULL_HANDLE;
			VmaAllocation m_Allocation = nullptr;

			VkImageLayout m_CurrentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		};
	}
}