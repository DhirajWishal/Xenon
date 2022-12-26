// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanSwapchain.hpp"
#include "VulkanMacros.hpp"

#if defined(XENON_PLATFORM_WINDOWS)
#include "../XenonPlatformWindows/WindowsWindow.hpp"

#endif // defined(XENON_PLATFORM_WINDOWS)

#include <optick.h>

namespace Xenon
{
	namespace Backend
	{
		VulkanSwapchain::VulkanSwapchain(VulkanDevice* pDevice, const std::string& title, uint32_t width, uint32_t height)
			: Swapchain(pDevice, title, width, height)
			, VulkanDeviceBoundObject(pDevice)
		{
			// Create the surface.
			createSurface();

			// Create the swapchain.
			createSwapchain();

			// Create the semaphores.
			setupSemaphores();
		}

		VulkanSwapchain::~VulkanSwapchain()
		{
			clear();

			for (const auto semaphore : m_RenderFinishedSemaphores)
				m_pDevice->getDeviceTable().vkDestroySemaphore(m_pDevice->getLogicalDevice(), semaphore, nullptr);

			for (const auto semaphore : m_InFlightSemaphores)
				m_pDevice->getDeviceTable().vkDestroySemaphore(m_pDevice->getLogicalDevice(), semaphore, nullptr);
		}

		uint32_t VulkanSwapchain::prepare()
		{
			OPTICK_EVENT();

			// If the application is minimized, return the previous image index.
			if (!isRenderable())
			{
				const auto capabilities = getSurfaceCapabilities();
				if (capabilities.currentExtent.width == 0 || capabilities.currentExtent.height == 0)
					return m_ImageIndex;

				recreate();
			}

			const auto result = m_pDevice->getDeviceTable().vkAcquireNextImageKHR(m_pDevice->getLogicalDevice(), m_Swapchain, 0, m_InFlightSemaphores[m_FrameIndex], VK_NULL_HANDLE, &m_ImageIndex);
			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
			{
				recreate();
				return prepare();
			}
			
			if (result == VK_TIMEOUT || result == VK_NOT_READY)
			{
				std::this_thread::sleep_for(std::chrono::microseconds(1));
				return prepare();
			}

			XENON_VK_ASSERT(result, "Failed to acquire the next swap chain image!");

			return m_ImageIndex;
		}

		void VulkanSwapchain::present()
		{
			OPTICK_EVENT();

			// Present if the application isn't minimized.
			if (isRenderable())
			{
				VkPresentInfoKHR presentInfo = {};
				presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
				presentInfo.pNext = nullptr;
				presentInfo.waitSemaphoreCount = 1;
				presentInfo.pWaitSemaphores = &m_RenderFinishedSemaphores[m_FrameIndex];
				presentInfo.swapchainCount = 1;
				presentInfo.pSwapchains = &m_Swapchain;
				presentInfo.pImageIndices = &m_ImageIndex;
				presentInfo.pResults = VK_NULL_HANDLE;

				// Present it to the surface.
				const auto result = m_pDevice->getTransferQueue().access([this](const VulkanQueue& queue, const VkPresentInfoKHR& presentInfo)
					{
						return m_pDevice->getDeviceTable().vkQueuePresentKHR(queue.getQueue(), &presentInfo);
					}
				, presentInfo);

				if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
					recreate();

				else
					XENON_VK_ASSERT(result, "Failed to present the swapchain image!");
			}

			// Increment the frame index.
			incrementFrame();
		}

		void VulkanSwapchain::recreate()
		{
			OPTICK_EVENT();

			clear();

			createSurface();
			createSwapchain();
		}

		void VulkanSwapchain::createSurface()
		{
			OPTICK_EVENT();

#if defined(XENON_PLATFORM_WINDOWS)
			VkWin32SurfaceCreateInfoKHR createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.hinstance = nullptr;
			createInfo.hwnd = static_cast<Platform::WindowsWindow*>(m_pWindow.get())->getWindowHandle();

			XENON_VK_ASSERT(vkCreateWin32SurfaceKHR(m_pDevice->getInstance()->getInstance(), &createInfo, nullptr, &m_Surface), "Failed to create the Windows surface!");

#else 
#error "Surface creation for the current platform is not supported!"

#endif // defined(XENON_PLATFORM_WINDOWS)
		}

		void VulkanSwapchain::createSwapchain()
		{
			OPTICK_EVENT();

			// Get the surface capabilities.
			const auto surfaceCapabilities = getSurfaceCapabilities();

			// Resolve the surface composite.
			auto surfaceComposite = static_cast<VkCompositeAlphaFlagBitsKHR>(surfaceCapabilities.supportedCompositeAlpha);
			if (surfaceComposite & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
				surfaceComposite = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

			else if (surfaceComposite & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
				surfaceComposite = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;

			else if (surfaceComposite & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR)
				surfaceComposite = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
			else
				surfaceComposite = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;

			m_FrameCount = std::clamp(surfaceCapabilities.minImageCount + 1, surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount);
			m_RenderWidth = surfaceCapabilities.currentExtent.width;
			m_RenderHeight = surfaceCapabilities.currentExtent.height;

			// Return if the window is minimized.
			if (!isRenderable())
				return;

			// Get the present modes.
			uint32_t presentModeCount = 0;
			XENON_VK_ASSERT(vkGetPhysicalDeviceSurfacePresentModesKHR(m_pDevice->getPhysicalDevice(), m_Surface, &presentModeCount, nullptr), "Failed to get the surface present mode count!");

			if (presentModeCount == 0)
			{
				XENON_LOG_FATAL("No suitable present formats found!");
				return;
			}

			std::vector<VkPresentModeKHR> presentModes(presentModeCount);
			XENON_VK_ASSERT(vkGetPhysicalDeviceSurfacePresentModesKHR(m_pDevice->getPhysicalDevice(), m_Surface, &presentModeCount, presentModes.data()), "Failed to get the surface present modes!");

			// Check if we have the present mode we need.
			VkPresentModeKHR presentMode = presentModes.front();
			for (const auto availablePresentMode : presentModes)
			{
				if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
					break;
				}
			}

			// Get the surface formats.
			uint32_t formatCount = 0;
			XENON_VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(m_pDevice->getPhysicalDevice(), m_Surface, &formatCount, nullptr), "Failed to get the surface format count!");

			if (formatCount == 0)
			{
				XENON_LOG_FATAL("No suitable surface formats found!");
				return;
			}

			std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
			XENON_VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(m_pDevice->getPhysicalDevice(), m_Surface, &formatCount, surfaceFormats.data()), "Failed to get the surface formats!");

			// Get the best surface format.
			VkSurfaceFormatKHR surfaceFormat = surfaceFormats.front();
			for (const auto& availableFormat : surfaceFormats)
			{
				if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				{
					surfaceFormat = availableFormat;
					break;
				}
			}

			m_SwapchainFormat = surfaceFormat.format;

			// Create the swapchain.
			VkSwapchainCreateInfoKHR createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.surface = m_Surface;
			createInfo.minImageCount = m_FrameCount;
			createInfo.imageFormat = m_SwapchainFormat;
			createInfo.imageColorSpace = surfaceFormat.colorSpace;
			createInfo.imageExtent = surfaceCapabilities.currentExtent;
			createInfo.imageArrayLayers = 1;
			createInfo.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
			createInfo.preTransform = surfaceCapabilities.currentTransform;
			createInfo.compositeAlpha = surfaceComposite;
			createInfo.presentMode = presentMode;
			createInfo.clipped = VK_TRUE;
			createInfo.oldSwapchain = VK_NULL_HANDLE;

			// Resolve the queue families if the two queues are different.
			const std::array<uint32_t, 2> queueFamilyindices = {
				m_pDevice->getGraphicsQueue().getUnsafe().getFamily(),
				m_pDevice->getTransferQueue().getUnsafe().getFamily()
			};

			if (queueFamilyindices[0] != queueFamilyindices[1])
			{
				createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
				createInfo.queueFamilyIndexCount = queueFamilyindices.size();
				createInfo.pQueueFamilyIndices = queueFamilyindices.data();
			}

			XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkCreateSwapchainKHR(m_pDevice->getLogicalDevice(), &createInfo, nullptr, &m_Swapchain), "Failed to create the swapchain!");

			// Get the image views.
			m_SwapchainImages.resize(m_FrameCount);
			XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkGetSwapchainImagesKHR(m_pDevice->getLogicalDevice(), m_Swapchain, &m_FrameCount, m_SwapchainImages.data()), "Failed to get the swapchain images!");

			// Finally we can resolve the swapchain image views.
			setupImageViews();
		}

		void VulkanSwapchain::setupImageViews()
		{
			OPTICK_EVENT();

			VkImageViewCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.pNext = VK_NULL_HANDLE;
			createInfo.flags = 0;
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = m_SwapchainFormat;
			createInfo.components = {};
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			m_SwapchainImageViews.resize(m_SwapchainImages.size());

			// Iterate over the image views and create them.
			VkImageView* pArray = m_SwapchainImageViews.data();
			for (auto itr = m_SwapchainImages.begin(); itr != m_SwapchainImages.end(); ++itr, ++pArray)
			{
				createInfo.image = *itr;
				XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkCreateImageView(m_pDevice->getLogicalDevice(), &createInfo, nullptr, pArray), "Failed to create the swapchain image view!");
			}
		}

		void VulkanSwapchain::clear()
		{
			OPTICK_EVENT();

			m_pDevice->getDeviceTable().vkDeviceWaitIdle(m_pDevice->getLogicalDevice());

			for (const auto view : m_SwapchainImageViews)
				m_pDevice->getDeviceTable().vkDestroyImageView(m_pDevice->getLogicalDevice(), view, nullptr);

			m_pDevice->getDeviceTable().vkDestroySwapchainKHR(m_pDevice->getLogicalDevice(), m_Swapchain, nullptr);
			vkDestroySurfaceKHR(m_pDevice->getInstance()->getInstance(), m_Surface, nullptr);

			m_SwapchainImageViews.clear();
			m_Swapchain = VK_NULL_HANDLE;
		}

		VkSurfaceCapabilitiesKHR VulkanSwapchain::getSurfaceCapabilities() const
		{
			OPTICK_EVENT();

			VkSurfaceCapabilitiesKHR capabilities = {};
			XENON_VK_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_pDevice->getPhysicalDevice(), m_Surface, &capabilities), "Failed to get the surface capabilities!");

			return capabilities;
		}

		void VulkanSwapchain::setupSemaphores()
		{
			VkSemaphoreCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;

			// Reserve to allocate everything once.
			m_RenderFinishedSemaphores.reserve(m_FrameCount);
			m_InFlightSemaphores.reserve(m_FrameCount);

			// Iterate over and create the semaphores.
			for (uint32_t i = 0; i < m_FrameCount; i++)
			{
				XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkCreateSemaphore(m_pDevice->getLogicalDevice(), &createInfo, nullptr, &m_RenderFinishedSemaphores.emplace_back()), "Failed to create the render finished semaphore!");
				XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkCreateSemaphore(m_pDevice->getLogicalDevice(), &createInfo, nullptr, &m_InFlightSemaphores.emplace_back()), "Failed to create the in flight semaphore!");
			}
		}
	}
}