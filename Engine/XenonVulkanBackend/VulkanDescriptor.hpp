// Copyright 2022-2023 Dhiraj Wishal
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
			 * @param bindingInfo The descriptor's binding information.
			 * @param type The descriptor type.
			 */
			explicit VulkanDescriptor(VulkanDevice* pDevice, const std::unordered_map<uint32_t, DescriptorBindingInfo>& bindingInfo, DescriptorType type);

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