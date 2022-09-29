// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/CommandBuffers.hpp"

#include "VulkanCommandBuffer.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan command buffers class.
		 */
		class VulkanCommandBuffers final : public CommandBuffers, public VulkanDeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The Vulkan device.
			 * @param bufferCount The number of command buffers to have.
			 */
			explicit VulkanCommandBuffers(VulkanDevice* pDevice, uint32_t bufferCount);

			/**
			 * Destructor.
			 */
			~VulkanCommandBuffers() override;

		private:
			std::vector<VulkanCommandBuffer> m_CommandBuffers;

			VkCommandPool m_CommandPool = VK_NULL_HANDLE;
		};
	}
}