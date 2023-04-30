// Copyright 2022-2023 Nexonous
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
			 * @param width The width of the render target.
			 * @param height The height of the render target.
			 */
			explicit VulkanRayTracer(VulkanDevice* pDevice, uint32_t width, uint32_t height);

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