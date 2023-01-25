// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanImage.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan image layout manager class.
		 * This class is intended to manage image layouts when certain GPU operations are completed.
		 */
		class VulkanImageLayoutManager : public VulkanDeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 */
			explicit VulkanImageLayoutManager(VulkanDevice* pDevice);

			/**
			 * Setup a image memory barrier.
			 *
			 * @param commandBuffer The Vulkan command buffer.
			 * @param imageBarrier The image barrier.
			 * @param srcStageFlags The source pipeline stage flags.
			 * @param dstStageFlags The destination pipeline stage flags.
			 */
			void barrier(VkCommandBuffer commandBuffer, const VkImageMemoryBarrier& imageBarrier, VkPipelineStageFlags srcStageFlags, VkPipelineStageFlags dstStageFlags) const;
		};
	}
}