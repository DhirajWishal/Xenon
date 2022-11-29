// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/Descriptor.hpp"

#include "VulkanDeviceBoundObject.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan descriptor class.
		 */
		class VulkanDescriptor final : public Descriptor, public VulkanDeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param bindingInfo The descriptor's binding information. Make sure that the binding information are in the binding order (the first one is binging 0, second is 1 and so on).
			 * @param type The descriptor type.
			 */
			explicit VulkanDescriptor(VulkanDevice* pDevice, const std::vector<DescriptorBindingInfo>& bindingInfo, DescriptorType type);

			/**
			 * Destructor.
			 */
			~VulkanDescriptor() override;

			/**
			 * Attach a buffer to the descriptor.
			 *
			 * @param binding The binding of the buffer.
			 * @param pBuffer The buffer to attach.
			 */
			void attach(uint32_t binding, Buffer* pBuffer) override;

			/**
			 * Attach an image to the descriptor.
			 *
			 * @param binding The binding of the image.
			 * @param pImage The image to attach.
			 * @param pView The image view.
			 * @param pSampler The image sampler.
			 * @param usage How the image is used in the binding.
			 */
			void attach(uint32_t binding, Image* pImage, ImageView* pView, ImageSampler* pSampler, ImageUsage usage) override;

			/**
			 * Get the descriptor set.
			 *
			 * @return The Vulkan descriptor set.
			 */
			[[nodiscard]] VkDescriptorSet getDescriptorSet() const noexcept { return m_DescriptorSet; }

		private:
			VkDescriptorPool m_Pool = VK_NULL_HANDLE;
			VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;
		};
	}
}