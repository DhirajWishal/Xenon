// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanRayTracer.hpp"
#include "VulkanMacros.hpp"

namespace /* anonymous */
{
	/**
	 * Get the image specification used to create the color image.
	 */
	[[nodiscard]] Xenon::Backend::ImageSpecification GetImageSpecification(const Xenon::Backend::Camera* pCamera) noexcept
	{
		Xenon::Backend::ImageSpecification specification;
		specification.m_Width = pCamera->getWidth();
		specification.m_Height = pCamera->getHeight();
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
		VulkanRayTracer::VulkanRayTracer(VulkanDevice* pDevice, Camera* pCamera)
			: RayTracer(pDevice, pCamera)
			, VulkanDeviceBoundObject(pDevice)
			, m_ColorImage(pDevice, GetImageSpecification(pCamera))
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