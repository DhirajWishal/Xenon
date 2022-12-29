// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/BottomLevelAccelerationStructure.hpp"

#include "VulkanBuffer.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan bottom level acceleration structure class.
		 */
		class VulkanBottomLevelAccelerationStructure final : public BottomLevelAccelerationStructure, public VulkanDeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param geometries The geometries to be stored in the acceleration structure.
			 */
			explicit VulkanBottomLevelAccelerationStructure(VulkanDevice* pDevice, const std::vector<AccelerationStructureGeometry>& geometries);

			/**
			 * Destructor.
			 */
			~VulkanBottomLevelAccelerationStructure() override;

		private:
			/**
			 * Create the acceleration structure.
			 *
			 * @param sizeInfo The acceleration structure's size information.
			 */
			void createAccelerationStructure(const VkAccelerationStructureBuildSizesInfoKHR& sizeInfo);

			/**
			 * Build the created acceleration structure.
			 *
			 * @param sizeInfo The acceleration structure's size information.
			 * @param geometries The geometries to be stored in the acceleration structure.
			 * @param triangleCount The number of triangles in the structure.
			 */
			void buildAccelerationStructure(const VkAccelerationStructureBuildSizesInfoKHR& sizeInfo, const std::vector<VkAccelerationStructureGeometryKHR>& geometries, uint32_t triangleCount);

		private:
			VkAccelerationStructureKHR  m_AccelerationStructure = VK_NULL_HANDLE;
			VkBuffer m_Buffer = VK_NULL_HANDLE;
			VkDeviceAddress m_DeviceAddress = 0;
			VmaAllocation m_Allocation = nullptr;
		};
	}
}