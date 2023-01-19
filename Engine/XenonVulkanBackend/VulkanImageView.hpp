// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/ImageView.hpp"

#include "VulkanImage.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan image view class.
		 */
		class VulkanImageView final : public ImageView, public VulkanDeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param pImage The image pointer.
			 * @param specification The view specification.
			 */
			explicit VulkanImageView(VulkanDevice* pDevice, VulkanImage* pImage, const ImageViewSpecification& specification);

			/**
			 * Destructor.
			 */
			~VulkanImageView() override;

			/**
			 * Get the image view.
			 *
			 * @return The image view.
			 */
			[[nodiscard]] VkImageView getView() const { return m_View; }

		private:
			VkImageView m_View = VK_NULL_HANDLE;
		};
	}
}