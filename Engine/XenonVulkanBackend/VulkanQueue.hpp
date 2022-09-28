// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <volk.h>

#include <optional>

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan queue class.
		 */
		class VulkanQueue final
		{
		public:
			/**
			 * Default constructor.
			 */
			VulkanQueue() = default;

			/**
			 * Default destructor.
			 */
			~VulkanQueue() = default;

			/**
			 * Setup the queue's family.
			 *
			 * @param physicalDevice The physical device to which the queue is bound to.
			 * @param flag The queue flag.
			 */
			void setupFamily(VkPhysicalDevice physicalDevice, VkQueueFlagBits flag);

		private:
			VkQueue m_Queue = VK_NULL_HANDLE;
			std::optional<uint32_t> m_Family;
		};
	}
}