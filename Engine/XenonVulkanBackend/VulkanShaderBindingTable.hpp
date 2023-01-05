// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/ShaderBindingTable.hpp"

#include "VulkanRayTracingPipeline.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan shader binding table class.
		 */
		class VulkanShaderBindingTable final : public ShaderBindingTable, public VulkanDeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param pPipeline The ray tracing pipeline pointer.
			 * @param bindingGroups The binding groups.
			 */
			explicit VulkanShaderBindingTable(VulkanDevice* pDevice, VulkanRayTracingPipeline* pPipeline, const std::vector<BindingGroup>& bindingGroups);

			/**
			 * Destructor.
			 */
			~VulkanShaderBindingTable() override;

		private:
			/**
			 * Map the table memory.
			 *
			 * @return The mapped memory pointer.
			 */
			[[nodiscard]] std::byte* map();

			/**
			 * Unmap the table memory.
			 */
			void unmap();

		private:
			VkBuffer m_Table = VK_NULL_HANDLE;
			VmaAllocation m_Allocation = nullptr;

			VkStridedDeviceAddressRegionKHR m_RayGenAddressRegion;
		};
	}
}