// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/Device.hpp"

#include "VulkanInstance.hpp"
#include "VulkanQueue.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan device class.
		 */
		class VulkanDevice final : public Device
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pInstance The Vulkan instance pointer.
			 * @param requiredRenderTargets The required render targets.
			 */
			explicit VulkanDevice(VulkanInstance* pInstance, RenderTargetType requiredRenderTargets);

			/**
			 * Destructor.
			 */
			~VulkanDevice() override;

		private:
			/**
			 * Select the required physical device.
			 */
			void selectPhysicalDevice();

		private:
			std::vector<const char*> m_DeviceExtensions;

			VkPhysicalDeviceProperties m_PhysicalDeviceProperties = {};

			VulkanInstance* m_pInstance = nullptr;

			VkDevice m_LogicalDevice = VK_NULL_HANDLE;
			VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;

			VulkanQueue m_ComputeQueue;
			VulkanQueue m_GraphicsQueue;
			VulkanQueue m_TransferQueue;
		};
	}
}