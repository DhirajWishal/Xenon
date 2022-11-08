// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/Swapchain.hpp"

#include "VulkanDeviceBoundObject.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan swapchain class.
		 */
		class VulkanSwapchain final : public VulkanDeviceBoundObject, public Swapchain
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param pWindow The window pointer.
			 */
			explicit VulkanSwapchain(VulkanDevice* pDevice, Platform::Window* pWindow);

			/**
			 * Destructor.
			 */
			~VulkanSwapchain() override;

		private:
			VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
		};
	}
}