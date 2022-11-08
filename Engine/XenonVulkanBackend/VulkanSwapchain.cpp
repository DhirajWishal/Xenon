// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanSwapchain.hpp"

namespace Xenon
{
	namespace Backend
	{
		VulkanSwapchain::VulkanSwapchain(VulkanDevice* pDevice, Platform::Window* pWindow)
			: VulkanDeviceBoundObject(pDevice)
			, Swapchain(pDevice, pWindow)
		{
		}

		VulkanSwapchain::~VulkanSwapchain()
		{
		}
	}
}