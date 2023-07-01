// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/ImageSampler.hpp"

#include "VulkanDeviceBoundObject.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan image sampler class.
		 */
		class VulkanImageSampler final : public ImageSampler, public VulkanDeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param specification The sampler specification.
			 */
			explicit VulkanImageSampler(VulkanDevice* pDevice, const ImageSamplerSpecification& specification);

			/**
			 * Destructor.
			 */
			~VulkanImageSampler() override;

			/**
			 * Get the image sampler.
			 *
			 * @return The image sampler.
			 */
			XENON_NODISCARD VkSampler getSampler() const { return m_Sampler; }

		private:
			VkSampler m_Sampler = VK_NULL_HANDLE;
		};
	}
}