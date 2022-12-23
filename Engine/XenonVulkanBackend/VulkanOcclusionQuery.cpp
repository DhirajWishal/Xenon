// Copyright 2022 Dhiraj Wishal
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

		const std::vector<uint64_t>& VulkanOcclusionQuery::getResults()
		{
			XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkGetQueryPoolResults(
				m_pDevice->getLogicalDevice(),
				m_QueryPool,
				0,
				2,
				m_Samples.size() * sizeof(uint64_t),
				m_Samples.data(),
				sizeof(uint64_t),
				VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT), "Failed to get the query pool results!");

			return m_Samples;
		}
	}
}