// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12RayTracer.hpp"
#include "DX12Macros.hpp"

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
		DX12RayTracer::DX12RayTracer(DX12Device* pDevice, uint32_t width, uint32_t height)
			: RayTracer(pDevice, width, height)
			, DX12DeviceBoundObject(pDevice)
			, m_ColorImage(pDevice, GetImageSpecification(width, height))
		{
		}

		Image* DX12RayTracer::getImageAttachment(AttachmentType type)
		{
			if (type == AttachmentType::Color)
				return &m_ColorImage;

			return nullptr;
		}
	}
}