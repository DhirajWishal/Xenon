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
			 * @param title The title of the window.
			 * @param width The window's width.
			 * @param height The window's height.
			 */
			explicit VulkanSwapchain(VulkanDevice* pDevice, const std::string& title, uint32_t width, uint32_t height);

			/**
			 * Destructor.
			 */
			~VulkanSwapchain() override;

		private:
			/**
			 * Create the surface.
			 */
			void createSurface();

		private:
			VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
			VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
		};
	}
}