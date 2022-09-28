// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanDevice.hpp"
#include "VulkanMacros.hpp"

#include <set>

namespace /* anonymous */
{
	/**
	 * Check if the physical device supports the required queues.
	 *
	 * @param physicalDevice The physical device to check.
	 * @param flag The queue flag to check.
	 * @return Whether or not the queues are supported.
	 */
	bool CheckQueueSupport(VkPhysicalDevice physicalDevice, VkQueueFlagBits flag)
	{
		// Get the queue family count.
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

		// Validate if we have queue families.
		if (queueFamilyCount == 0)
		{
			XENON_LOG_FATAL("Failed to get the queue family property count!");
			return false;
		}

		// Get the queue family properties.
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

		// Iterate over those queue family properties and check if we have a family with the required flag.
		for (const auto& family : queueFamilies)
		{
			if (family.queueCount == 0)
				continue;

			// Check if the queue flag contains what we want.
			if (family.queueFlags & flag)
				return true;
		}

		return false;
	}

	/**
	 * Check device extension support.
	 *
	 * @param physicalDevice The physical device to check.
	 * @param deviceExtensions The extension to check.
	 * @param supportedTypes The variable to store the supported render target types.
	 * @return True if the device supports at least one of the device extensions.
	 * @return False if the device does not support any of the required extensions.
	 */
	bool CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice, const std::vector<const char*>& deviceExtensions, Xenon::RenderTargetType* supportedTypes = nullptr)
	{
		// If there are no extension to check, we can just return true.
		if (deviceExtensions.empty())
			return true;

		// Get the extension count.
		uint32_t extensionCount = 0;
		XENON_VK_ASSERT(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr), "Failed to enumerate physical device extension property count!");

		// Load the extensions.
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		XENON_VK_ASSERT(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data()), "Failed to enumerate physical device extension properties!");

		std::set<std::string_view> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		// Iterate and check if it contains the extensions we need. If it does, remove them from the set so we can later check if 
		// all the required extensions exist.
		for (const VkExtensionProperties& extension : availableExtensions)
			requiredExtensions.erase(extension.extensionName);

		// If the required extension count did not change, we don't support any of those required extensions.
		if (requiredExtensions.size() == deviceExtensions.size())
		{
			XENON_LOG_INFORMATION("The physical device {} does not support any of the required extensions.", fmt::ptr(physicalDevice));
			return false;
		}

		// If the extension count is more than 0, that means it supports a few of those required extensions.
		if (requiredExtensions.size() > 0)
			XENON_LOG_INFORMATION("The physical device {} supports only some of the required extensions.");

		// Set the supported types if required.
		if (supportedTypes)
		{
			if (requiredExtensions.contains(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME)
				&& requiredExtensions.contains(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME)
				/* && requiredExtensions.contains(VK_KHR_RAY_QUERY_EXTENSION_NAME)*/)
			{
				*supportedTypes = Xenon::RenderTargetType::All;
			}
		}

		return true;
	}
}

namespace Xenon
{
	namespace Backend
	{
		VulkanDevice::VulkanDevice(VulkanInstance* pInstance, RenderTargetType requiredRenderTargets)
			: Device(pInstance, requiredRenderTargets)
			, m_pInstance(pInstance)
		{
			// Set up the device extensions.
			m_DeviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
			m_DeviceExtensions.emplace_back(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);

			// If the user needs ray tracing, we need to enable the following extensions.
			if (requiredRenderTargets & (RenderTargetType::PathTracer | RenderTargetType::RayTracer))
			{
				m_DeviceExtensions.emplace_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
				m_DeviceExtensions.emplace_back(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
				// m_DeviceExtensions.emplace_back(VK_KHR_RAY_QUERY_EXTENSION_NAME);
			}

			// Select the physical device.
			selectPhysicalDevice();

			// Create the logical device.
			createLogicalDevice();
		}

		VulkanDevice::~VulkanDevice()
		{
			m_DeviceTable.vkDestroyDevice(m_LogicalDevice, nullptr);
		}

		void VulkanDevice::selectPhysicalDevice()
		{
			// Enumerate physical devices.
			uint32_t deviceCount = 0;
			XENON_VK_ASSERT(vkEnumeratePhysicalDevices(m_pInstance->getInstance(), &deviceCount, nullptr), "Failed to enumerate physical devices.");

			// Throw an error if there are no physical devices available.
			if (deviceCount == 0)
			{
				XENON_LOG_FATAL("No physical devices found!");
				return;
			}

			std::vector<VkPhysicalDevice> candidates(deviceCount);
			XENON_VK_ASSERT(vkEnumeratePhysicalDevices(m_pInstance->getInstance(), &deviceCount, candidates.data()), "Failed to enumerate physical devices.");

			struct Candidate { VkPhysicalDeviceProperties m_Properties; VkPhysicalDevice m_Candidate; };
			std::array<Candidate, 6> priorityMap = { Candidate() };

			// Iterate through all the candidate devices and find the best device.
			for (const auto& candidate : candidates)
			{
				// Check if the device is suitable for our use.
				if (CheckDeviceExtensionSupport(candidate, m_DeviceExtensions) &&
					CheckQueueSupport(candidate, VK_QUEUE_GRAPHICS_BIT) &&
					CheckQueueSupport(candidate, VK_QUEUE_COMPUTE_BIT) &&
					CheckQueueSupport(candidate, VK_QUEUE_TRANSFER_BIT))
				{
					VkPhysicalDeviceProperties physicalDeviceProperties = {};
					vkGetPhysicalDeviceProperties(candidate, &physicalDeviceProperties);

					// Sort the candidates by priority.
					uint8_t priorityIndex = 5;
					switch (physicalDeviceProperties.deviceType)
					{
					case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
						priorityIndex = 0;
						break;

					case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
						priorityIndex = 1;
						break;

					case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
						priorityIndex = 2;
						break;

					case VK_PHYSICAL_DEVICE_TYPE_CPU:
						priorityIndex = 3;
						break;

					case VK_PHYSICAL_DEVICE_TYPE_OTHER:
						priorityIndex = 4;
						break;

					default:
						priorityIndex = 5;
						break;
					}

					priorityMap[priorityIndex].m_Candidate = candidate;
					priorityMap[priorityIndex].m_Properties = physicalDeviceProperties;
				}
			}

			// Choose the physical device with the highest priority.
			for (const auto& candidate : priorityMap)
			{
				if (candidate.m_Candidate != VK_NULL_HANDLE)
				{
					m_PhysicalDevice = candidate.m_Candidate;
					m_PhysicalDeviceProperties = candidate.m_Properties;
					break;
				}
			}

			// Check if we found a physical device.
			if (m_PhysicalDevice == VK_NULL_HANDLE)
			{
				XENON_LOG_FATAL("Could not find a physical device with the required requirements!");
				return;
			}

			// Get the supported render target types.
			CheckDeviceExtensionSupport(m_PhysicalDevice, m_DeviceExtensions, &m_SupportedRenderTargetTypes);

			// Setup the queue families.
			m_ComputeQueue.setupFamily(m_PhysicalDevice, VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT);
			m_GraphicsQueue.setupFamily(m_PhysicalDevice, VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT);
			m_TransferQueue.setupFamily(m_PhysicalDevice, VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT);
		}

		void VulkanDevice::createLogicalDevice()
		{
			// Setup device queues.
			constexpr float priority = 1.0f;
			std::set<uint32_t> uniqueQueueFamilies = {
				m_GraphicsQueue.getFamily(),
				m_ComputeQueue.getFamily(),
				m_TransferQueue.getFamily()
			};

			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.pNext = nullptr;
			queueCreateInfo.flags = 0;
			queueCreateInfo.queueFamilyIndex = 0;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &priority;

			std::vector< VkDeviceQueueCreateInfo> queueCreateInfos;
			for (const auto& family : uniqueQueueFamilies)
			{
				queueCreateInfo.queueFamilyIndex = family;
				queueCreateInfos.emplace_back(queueCreateInfo);
			}

			// Setup all the required features.
			VkPhysicalDeviceFeatures features = {};
			features.samplerAnisotropy = VK_TRUE;
			features.sampleRateShading = VK_TRUE;
			features.tessellationShader = VK_TRUE;
			features.geometryShader = VK_TRUE;

			// Setup the device create info.
			VkDeviceCreateInfo deviceCreateInfo = {};
			deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			deviceCreateInfo.pNext = nullptr;
			deviceCreateInfo.flags = 0;
			deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
			deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
			deviceCreateInfo.enabledLayerCount = 0;
			deviceCreateInfo.ppEnabledLayerNames = nullptr;
			deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
			deviceCreateInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();
			deviceCreateInfo.pEnabledFeatures = &features;

#ifdef XENON_DEBUG
			// Get the validation layers and initialize it.
			deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_pInstance->getValidationLayers().size());
			deviceCreateInfo.ppEnabledLayerNames = m_pInstance->getValidationLayers().data();

#endif // XENON_DEBUG

			// Create the device.
			XENON_VK_ASSERT(vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_LogicalDevice), "Failed to create the logical device!");

			// Load the device table.
			volkLoadDeviceTable(&m_DeviceTable, m_LogicalDevice);

			// Get the queues.
			VkQueue queue = VK_NULL_HANDLE;
			m_DeviceTable.vkGetDeviceQueue(m_LogicalDevice, m_GraphicsQueue.getFamily(), 0, &queue);
			m_GraphicsQueue.setQueue(queue);

			m_DeviceTable.vkGetDeviceQueue(m_LogicalDevice, m_ComputeQueue.getFamily(), 0, &queue);
			m_ComputeQueue.setQueue(queue);

			m_DeviceTable.vkGetDeviceQueue(m_LogicalDevice, m_TransferQueue.getFamily(), 0, &queue);
			m_TransferQueue.setQueue(queue);
		}
	}
}