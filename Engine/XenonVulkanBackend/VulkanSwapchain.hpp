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

			/**
			 * Create the swapchain.
			 */
			void createSwapchain();

			/**
			 * Setup the image views.
			 */
			void setupImageViews();

			/**
			 * Clear everything.
			 * This is needed when resizing.
			 */
			void clear();

			/**
			 * Get the surface capabilities.
			 *
			 * @return The surface capabilities.
			 */
			[[nodiscard]] VkSurfaceCapabilitiesKHR getSurfaceCapabilities() const;

		private:
			std::vector<VkImage> m_SwapchainImages;
			std::vector<VkImageView> m_SwapchainImageViews;

			VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
			VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;

			VkFormat m_SwapchainFormat = VK_FORMAT_UNDEFINED;
		};
	}
}