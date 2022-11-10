// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanSwapchain.hpp"
#include "VulkanMacros.hpp"

#if defined(XENON_PLATFORM_WINDOWS)
#include "../XenonPlatformWindows/WindowsWindow.hpp"

#endif // defined(XENON_PLATFORM_WINDOWS)

namespace Xenon
{
	namespace Backend
	{
		VulkanSwapchain::VulkanSwapchain(VulkanDevice* pDevice, const std::string& title, uint32_t width, uint32_t height)
			: VulkanDeviceBoundObject(pDevice)
			, Swapchain(pDevice, title, width, height)
		{
			// Create the surface.
			createSurface();
		}

		VulkanSwapchain::~VulkanSwapchain()
		{
			vkDestroySurfaceKHR(m_pDevice->getInstance()->getInstance(), m_Surface, nullptr);
		}

		void VulkanSwapchain::createSurface()
		{
#if defined(XENON_PLATFORM_WINDOWS)
			VkWin32SurfaceCreateInfoKHR createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.hinstance = nullptr;
			createInfo.hwnd = static_cast<Platform::WindowsWindow*>(m_pWindow.get())->getWindowHandle();

			XENON_VK_ASSERT(vkCreateWin32SurfaceKHR(m_pDevice->getInstance()->getInstance(), &createInfo, nullptr, &m_Surface), "Failed to create the Windows surface!");
#endif
		}
	}
}