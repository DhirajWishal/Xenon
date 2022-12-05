// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Buffer.hpp"
#include "ImageView.hpp"
#include "ImageSampler.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Descriptor class.
		 * Descriptors define how to attach resources to shaders.
		 */
		class Descriptor : public BackendObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param bindingInfo The descriptor's binding information. Make sure that the binding information are in the binding order (the first one is binging 0, second is 1 and so on).
			 * @param type The descriptor type.
			 */
			explicit Descriptor([[maybe_unused]] const Device* pDevice, const std::vector<DescriptorBindingInfo>& bindingInfo, DescriptorType type) : m_BindingInformation(bindingInfo), m_Type(type) {}

			/**
			 * Attach a buffer to the descriptor.
			 *
			 * @param binding The binding of the buffer.
			 * @param pBuffer The buffer to attach.
			 */
			virtual void attach(uint32_t binding, Buffer* pBuffer) = 0;

			/**
			 * Attach an image to the descriptor.
			 *
			 * @param binding The binding of the image.
			 * @param pImage The image to attach.
			 * @param pView The image view.
			 * @param pSampler The image sampler.
			 * @param usage How the image is used in the binding.
			 */
			virtual void attach(uint32_t binding, Image* pImage, ImageView* pView, ImageSampler* pSampler, ImageUsage usage) = 0;

			/**
			 * Get the descriptor set's binding information.
			 *
			 * @return The binding information vector.
			 */
			[[nodiscard]] const std::vector<DescriptorBindingInfo>& getBindingInfo() const { return m_BindingInformation; }

			/**
			 * Get the descriptor set type.
			 *
			 * @return The descriptor set type.
			 */
			[[nodiscard]] DescriptorType getType() const { return m_Type; }

		protected:
			std::vector<DescriptorBindingInfo> m_BindingInformation;
			DescriptorType m_Type = DescriptorType::UserDefined;
		};
	}
}