// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/Instance.hpp"

#if defined(XENON_PLATFORM_WINDOWS)
#define VK_USE_PLATFORM_WIN32_KHR

#endif // defined(XENON_PLATFORM_WINDOWS)

#include <volk.h>

#include <fstream>

namespace Xenon
{
	namespace Backend
	{
		/**
		 * This is the Vulkan API version used by the engine.
		 */
		constexpr auto VulkanVersion = VK_API_VERSION_1_3;

		/**
		 * Instance class.
		 * This is the base class for the backend instance.
		 */
		class VulkanInstance final : public Instance
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param appliationName The name of the application.
			 * @param applicationVersion The application version.
			 */
			explicit VulkanInstance(const std::string& applicationName, uint32_t applicationVersion);

			/**
			 * Destructor.
			 */
			~VulkanInstance() override;

		public:
			/**
			 * Get the instance handle.
			 *
			 * @return The Vulkan instance handle.
			 */
			[[nodiscard]] VkInstance getInstance() const { return m_Instance; }

			/**
			 * Get the validation layers.
			 *
			 * @return The validation layers.
			 */
			[[nodiscard]] const std::vector<const char*>& getValidationLayers() const { return m_pValidationLayers; }

			/**
			 * Get the Vulkan's log file.
			 * This is file is per-instance and is generated at runtime.
			 *
			 * @return The log file.
			 */
			[[nodiscard]] std::ofstream& getLogFile();

		private:
			/**
			 * Create the Vulkan instance.
			 *
			 * @param applicationName The name of the application.
			 * @param applicationVersion The application version.
			 */
			void createInstance(const std::string& applicationName, uint32_t applicationVersion);

		private:
			std::ofstream m_LogFile;
			std::vector<const char*> m_pValidationLayers;

			VkInstance m_Instance = VK_NULL_HANDLE;
			VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
		};
	}
}