// Copyright 2022-2023 Nexonous
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
		class VulkanSwapchain final : public Swapchain, public VulkanDeviceBoundObject
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

			/**
			 * Prepare the swapchain to present.
			 *
			 * @return The swapchain image index.
			 */
			[[nodiscard]] uint32_t prepare() override;

			/**
			 * Present the swapchain to the window.
			 */
			void present() override;

			/**
			 * Recreate the swapchain.
			 * This is called internally when the window is resized or by any other resizing event.
			 */
			void recreate() override;

			/**
			 * Get the in-flight semaphore pointer.
			 *
			 * @return The semaphore pointer.
			 */
			[[nodiscard]] VkSemaphore* getInFlightSemaphorePtr() { return &m_InFlightSemaphores[m_FrameIndex]; }

			/**
			 * Get the in-flight semaphore pointer.
			 *
			 * @return The const semaphore pointer.
			 */
			[[nodiscard]] const VkSemaphore* getInFlightSemaphorePtr() const { return &m_InFlightSemaphores[m_FrameIndex]; }

			/**
			 * Get the render-finished semaphore pointer.
			 *
			 * @return The semaphore pointer.
			 */
			[[nodiscard]] VkSemaphore* getRenderFinishedSemaphorePtr() { return &m_RenderFinishedSemaphores[m_FrameIndex]; }

			/**
			 * Get the render-finished semaphore pointer.
			 *
			 * @return The const semaphore pointer.
			 */
			[[nodiscard]] const VkSemaphore* getRenderFinishedSemaphorePtr() const { return &m_RenderFinishedSemaphores[m_FrameIndex]; }

			/**
			 * Get the current swapchain image.
			 *
			 * @return The current swapchain image.
			 */
			[[nodiscard]] VkImage getCurrentImage() const { return m_SwapchainImages[m_ImageIndex]; }

			/**
			 * Get the render width.
			 * Render width is the actual image size of the swapchain.
			 *
			 * @return The width.
			 */
			[[nodiscard]] uint32_t getRenderWidth() const { return m_RenderWidth; }

			/**
			 * Get the render height.
			 * Render height is the actual image size of the swapchain.
			 *
			 * @return The height.
			 */
			[[nodiscard]] uint32_t getRenderHeight() const { return m_RenderHeight; }

			/**
			 * Check if the swapchain is renderable.
			 * This becomes false when the render width or height becomes zero.
			 *
			 * @return True if the swapchain is renderable.
			 * @return False if the swapchain is not renderable.
			 */
			[[nodiscard]] bool isRenderable() const { return m_RenderWidth > 0 && m_RenderHeight > 0; }

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

			/**
			 * Setup the semaphores used for synchronization.
			 */
			void setupSemaphores();

		private:
			std::vector<VkImage> m_SwapchainImages;
			std::vector<VkImageView> m_SwapchainImageViews;

			std::vector<VkSemaphore> m_RenderFinishedSemaphores;
			std::vector<VkSemaphore> m_InFlightSemaphores;

			VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
			VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;

			uint32_t m_RenderWidth = 0;
			uint32_t m_RenderHeight = 0;

			VkFormat m_SwapchainFormat = VK_FORMAT_UNDEFINED;
		};
	}
}