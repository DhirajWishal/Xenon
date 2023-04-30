// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#include "VulkanRayTracer.hpp"
#include "VulkanMacros.hpp"

namespace /* anonymous */
{
	/**
	 * Get the image specification used to create the color image.
	 */
	[[nodiscard]] Xenon::Backend::ImageSpecification GetImageSpecification(uint32_t width, uint32_t height) noexcept
	{
		Xenon::Backend::ImageSpecification specification;
		specification.m_Width = width;
		specification.m_Height = height;
		specification.m_Usage = Xenon::Backend::ImageUsage::ColorAttachment | Xenon::Backend::ImageUsage::Storage;
		specification.m_Format = Xenon::Backend::DataFormat::R8G8B8A8_UNORMAL | Xenon::Backend::DataFormat::R8G8B8A8_SRGB;
		specification.m_EnableMipMaps = false;

		return specification;
	}
}

namespace Xenon
{
	namespace Backend
	{
		VulkanRayTracer::VulkanRayTracer(VulkanDevice* pDevice, uint32_t width, uint32_t height)
			: RayTracer(pDevice, width, height)
			, VulkanDeviceBoundObject(pDevice)
			, m_ColorImage(pDevice, GetImageSpecification(width, height))
		{
		}

		Image* VulkanRayTracer::getImageAttachment(AttachmentType type)
		{
			if (type == AttachmentType::Color)
				return &m_ColorImage;

			return nullptr;
		}
	}
}