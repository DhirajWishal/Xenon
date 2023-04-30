// Copyright 2022-2023 Nexonous
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

			/**
			 * Get the ray generation address range.
			 *
			 * @return The address range.
			 */
			[[nodiscard]] VkStridedDeviceAddressRegionKHR getRayGenerationAddressRegion() const noexcept { return m_RayGenAddressRegion; }

			/**
			 * Get the miss address range.
			 *
			 * @return The address range.
			 */
			[[nodiscard]] VkStridedDeviceAddressRegionKHR getMissAddressRegion() const noexcept { return m_MissAddressRegion; }

			/**
			 * Get the ray hit address range.
			 *
			 * @return The address range.
			 */
			[[nodiscard]] VkStridedDeviceAddressRegionKHR getHitAddressRegion() const noexcept { return m_HitAddressRegion; }

			/**
			 * Get the callable address range.
			 *
			 * @return The address range.
			 */
			[[nodiscard]] VkStridedDeviceAddressRegionKHR getCallableAddressRegion() const noexcept { return m_CallableAddressRegion; }

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

			/**
			 * Get the table's device address.
			 *
			 * @return The device address.
			 */
			[[nodiscard]] VkDeviceAddress getDeviceAddress() const;

		private:
			VkBuffer m_Table = VK_NULL_HANDLE;
			VmaAllocation m_Allocation = nullptr;

			VkStridedDeviceAddressRegionKHR m_RayGenAddressRegion;
			VkStridedDeviceAddressRegionKHR m_MissAddressRegion;
			VkStridedDeviceAddressRegionKHR m_HitAddressRegion;
			VkStridedDeviceAddressRegionKHR m_CallableAddressRegion;
		};
	}
}