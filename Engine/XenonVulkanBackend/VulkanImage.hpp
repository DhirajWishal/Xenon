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

		public:
			/**
			 * Move assignment operator.
			 *
			 * @param other The other image.
			 * @return The move-assigned image.
			 */
			VulkanImage& operator=(VulkanImage&& other) noexcept;

		private:
			VkImage m_Image = VK_NULL_HANDLE;
			VmaAllocation m_Allocation = nullptr;
		};
	}
}