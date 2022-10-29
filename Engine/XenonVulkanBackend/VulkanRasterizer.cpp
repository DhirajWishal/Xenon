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
			if (attachmentTypes & AttachmentType::Color)
			{
				// Create and add the color attachment.
			}

			if (attachmentTypes & AttachmentType::EntityID)
			{
				// Create and add the entity ID attachment.
			}

			if (attachmentTypes & AttachmentType::Normal)
			{
				// Create and add the normal attachment.
			}

			if (attachmentTypes & AttachmentType::Depth && attachmentTypes & AttachmentType::Stencil)
			{
				// Create and add the depth attachment with stencil attachment.
			}

			else if (attachmentTypes & AttachmentType::Depth)
			{
				// Create and add the depth attachment.
			}

			else if (attachmentTypes & AttachmentType::Stencil)
			{
				// Create and add the stencil attachment.
			}
		}
	}
}