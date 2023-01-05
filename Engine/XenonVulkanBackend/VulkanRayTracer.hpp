// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/RayTracer.hpp"

#include "VulkanImage.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan ray tracer class.
		 */
		class VulkanRayTracer final : public RayTracer, public VulkanDeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param pCamera The camera pointer.
			 */
			explicit VulkanRayTracer(VulkanDevice* pDevice, Camera* pCamera);

			/**
			 * Destructor.
			 */
			~VulkanRayTracer() override = default;

			/**
			 * Get the image attachment of the relevant attachment type.
			 *
			 * @param type The attachment type.
			 * @return The attachment image.
			 */
			[[nodiscard]] Image* getImageAttachment(AttachmentType type) override;

		private:
			VulkanImage m_ColorImage;
		};
	}
}