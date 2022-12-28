// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanRayTracer.hpp"
#include "VulkanMacros.hpp"

namespace Xenon
{
	namespace Backend
	{
		VulkanRayTracer::VulkanRayTracer(VulkanDevice* pDevice, Camera* pCamera)
			: RayTracer(pDevice, pCamera)
			, VulkanDeviceBoundObject(pDevice)
		{

		}
	}
}