// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#include "VulkanImageLayoutManager.hpp"

namespace Xenon
{
	namespace Backend
	{
		VulkanImageLayoutManager::VulkanImageLayoutManager(VulkanDevice* pDevice)
			: VulkanDeviceBoundObject(pDevice)
		{
		}

		void VulkanImageLayoutManager::barrier(VkCommandBuffer commandBuffer, const VkImageMemoryBarrier& imageBarrier, VkPipelineStageFlags srcStageFlags, VkPipelineStageFlags dstStageFlags) const
		{
			m_pDevice->getDeviceTable().vkCmdPipelineBarrier(
				commandBuffer,
				srcStageFlags, dstStageFlags, 0,
				0, nullptr,
				0, nullptr,
				1, &imageBarrier
			);
		}
	}
}