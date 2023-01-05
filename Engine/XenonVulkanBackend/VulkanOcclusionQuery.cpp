// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanOcclusionQuery.hpp"
#include "VulkanMacros.hpp"

namespace Xenon
{
	namespace Backend
	{
		VulkanOcclusionQuery::VulkanOcclusionQuery(VulkanDevice* pDevice, uint64_t sampleCount)
			: OcclusionQuery(pDevice, sampleCount)
			, VulkanDeviceBoundObject(pDevice)
		{
			VkQueryPoolCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
			createInfo.queryType = VK_QUERY_TYPE_OCCLUSION;
			createInfo.queryCount = static_cast<uint32_t>(sampleCount);
			XENON_VK_ASSERT(pDevice->getDeviceTable().vkCreateQueryPool(pDevice->getLogicalDevice(), &createInfo, nullptr, &m_QueryPool), "Failed to create the occlusion query!");
		}

		VulkanOcclusionQuery::~VulkanOcclusionQuery()
		{
			m_pDevice->getDeviceTable().vkDestroyQueryPool(m_pDevice->getLogicalDevice(), m_QueryPool, nullptr);
		}
	}
}