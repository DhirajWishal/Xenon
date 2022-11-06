// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanQueue.hpp"
#include "VulkanMacros.hpp"

namespace Xenon
{
	namespace Backend
	{
		void VulkanQueue::setupFamily(VkPhysicalDevice physicalDevice, VkQueueFlagBits flag)
		{
			// Get the queue family count.
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

			// Validate if we have queue families.
			if (queueFamilyCount == 0)
			{
				XENON_LOG_FATAL("Failed to get the queue family property count!");
				return;
			}

			// Get the queue family properties.
			std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

			// Iterate over those queue family properties and check if we have a family with the required flag.
			for (uint32_t i = 0; i < queueFamilies.size(); ++i)
			{
				const auto& family = queueFamilies[i];
				if (family.queueCount == 0)
					continue;

				// Check if the queue flag contains what we want.
				if (family.queueFlags & flag)
				{
					m_Family = i;
					break;
				}
			}
		}

		void VulkanQueue::setQueue(VkQueue queue)
		{
			m_Queue = queue;
		}
	}
}