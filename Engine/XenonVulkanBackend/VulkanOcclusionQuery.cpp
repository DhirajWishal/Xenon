// Copyright 2022-2023 Nexonous
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

		std::vector<uint64_t> VulkanOcclusionQuery::getSamples()
		{
			auto samples = std::vector<uint64_t>(m_SampleCount);

			const auto result = m_pDevice->getDeviceTable().vkGetQueryPoolResults(
				m_pDevice->getLogicalDevice(),
				getQueryPool(),
				0,
				static_cast<uint32_t>(m_SampleCount),
				m_SampleCount * sizeof(uint64_t),
				samples.data(),
				sizeof(uint64_t),
				VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_PARTIAL_BIT);

			if (result != VK_NOT_READY)
				XENON_VK_ASSERT(result, "Failed to get the query pool results!");

			return samples;
		}
	}
}