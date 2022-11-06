// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanRasterizer.hpp"

namespace Xenon
{
	namespace Backend
	{
		VulkanRasterizer::VulkanRasterizer(VulkanDevice* pDevice, Camera* pCamera, AttachmentType attachmentTypes, bool enableTripleBuffering /*= false*/, MultiSamplingCount multiSampleCount /*= MultiSamplingCount::x1*/)
			: Rasterizer(pDevice, pCamera, attachmentTypes, enableTripleBuffering, multiSampleCount)
			, VulkanDeviceBoundObject(pDevice)
		{
			// Setup the image attachments.
			setupAttachments();
		}

		void VulkanRasterizer::setupAttachments()
		{
			ImageSpecification specification;
			specification.m_Width = m_pCamera->getWidth();
			specification.m_Height = m_pCamera->getHeight();
			specification.m_Depth = 1;
			specification.m_EnableMipMaps = false;

			// The few attachments below are color attachments.
			specification.m_Usage = ImageUsage::ColorAttachment | ImageUsage::Storage;

			// Create and add the color attachment if required.
			if (m_AttachmentTypes & AttachmentType::Color)
			{
				specification.m_Format = DataFormat::R8G8B8A8_UNORMAL | DataFormat::R8G8B8A8_SRGB;
				specification.m_MultiSamplingCount = m_MultiSamplingCount;

				m_ImageAttachments.emplace_back(m_pDevice, specification);
			}

			// The rest of the attachments don't need multi-sampling.
			specification.m_MultiSamplingCount = MultiSamplingCount::x1;

			// Create and add the entity ID attachment if required.
			if (m_AttachmentTypes & AttachmentType::EntityID)
			{
				specification.m_Format = DataFormat::R32_SFLOAT;
				m_ImageAttachments.emplace_back(m_pDevice, specification);
			}

			// Create and add the normal attachment if required.
			if (m_AttachmentTypes & AttachmentType::Normal)
			{
				specification.m_Format = DataFormat::R32G32B32_SFLOAT;
				m_ImageAttachments.emplace_back(m_pDevice, specification);
			}

			// The rest are depth attachments.
			specification.m_Usage = ImageUsage::DepthAttachment | ImageUsage::Storage;

			// Create and add the depth attachment with stencil attachment if required.
			if (m_AttachmentTypes & AttachmentType::Depth && m_AttachmentTypes & AttachmentType::Stencil)
			{
				specification.m_Format = DataFormat::D32_SFLOAT_S8_UINT | DataFormat::D24_UNORMAL_S8_UINT | DataFormat::D16_UNORMAL_S8_UINT;
				m_ImageAttachments.emplace_back(m_pDevice, specification);
			}

			// Create and add the depth attachment if required.
			else if (m_AttachmentTypes & AttachmentType::Depth)
			{
				specification.m_Format = DataFormat::D32_SFLOAT | DataFormat::D16_SINT;
				m_ImageAttachments.emplace_back(m_pDevice, specification);
			}

			// Create and add the stencil attachment if required.
			else if (m_AttachmentTypes & AttachmentType::Stencil)
			{
				specification.m_Format = DataFormat::S8_UINT;
				m_ImageAttachments.emplace_back(m_pDevice, specification);
			}
		}
	}
}