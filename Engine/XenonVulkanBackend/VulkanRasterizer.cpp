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
		}
	}
}