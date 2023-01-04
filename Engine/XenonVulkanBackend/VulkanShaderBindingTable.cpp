// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanShaderBindingTable.hpp"
#include "VulkanMacros.hpp"

namespace Xenon
{
	namespace Backend
	{
		VulkanShaderBindingTable::VulkanShaderBindingTable(VulkanDevice* pDevice, VulkanRayTracingPipeline* pPipeline, const std::vector<BindingGroup>& bindingGroups)
			: ShaderBindingTable(pDevice, pPipeline, bindingGroups)
			, VulkanDeviceBoundObject(pDevice)
		{
			const auto handleSize = m_pDevice->getPhysicalDeviceRayTracingPipelineProperties().shaderGroupHandleSize;
			const auto handleAlignment = m_pDevice->getPhysicalDeviceRayTracingPipelineProperties().shaderGroupHandleAlignment;

			const uint32_t handleSizeAligned = XENON_VK_ALIGNED_SIZE(handleSize, handleAlignment);
			const auto groupCount = static_cast<uint32_t>(bindingGroups.size());
			const uint32_t sbtSize = groupCount * handleSizeAligned;

			std::vector<std::byte> shaderHandleStorage(sbtSize);
			XENON_VK_ASSERT(vkGetRayTracingShaderGroupHandlesKHR(m_pDevice->getLogicalDevice(), pPipeline->getPipeline(), 0, groupCount, sbtSize, shaderHandleStorage.data()));

			// auto ptr = shaderHandleStorage.data();
			// writeShaderGroupHandles(m_RayGenBindingTable.m_pTable.get(), ptr, handleSize, handleSizeAligned);
			// writeShaderGroupHandles(m_IntersectionBindingTable.m_pTable.get(), ptr, handleSize, handleSizeAligned);
			// writeShaderGroupHandles(m_AnyHitBindingTable.m_pTable.get(), ptr, handleSize, handleSizeAligned);
			// writeShaderGroupHandles(m_ClosestHitBindingTable.m_pTable.get(), ptr, handleSize, handleSizeAligned);
			// writeShaderGroupHandles(m_MissBindingTable.m_pTable.get(), ptr, handleSize, handleSizeAligned);
			// writeShaderGroupHandles(m_CallableBindingTable.m_pTable.get(), ptr, handleSize, handleSizeAligned);
		}

		VulkanShaderBindingTable::~VulkanShaderBindingTable()
		{
			vmaDestroyBuffer(m_pDevice->getAllocator(), m_Table, m_Allocation);
		}
	}
}