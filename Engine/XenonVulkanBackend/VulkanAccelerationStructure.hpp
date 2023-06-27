// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanDeviceBoundObject.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan acceleration structure class.
		 * This class is the base class for all the Vulkan acceleration structure objects.
		 */
		class VulkanAccelerationStructure : public VulkanDeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 */
			explicit VulkanAccelerationStructure(VulkanDevice* pDevice);

			/**
			 * Virtual destructor.
			 */
			virtual ~VulkanAccelerationStructure() override;

			/**
			 * Get the device address.
			 *
			 * @return The acceleration structure's device address.
			 */
			XENON_NODISCARD VkDeviceAddress getDeviceAddress() const noexcept { return m_DeviceAddress; }

		protected:
			/**
			 * Create the acceleration structure.
			 *
			 * @param sizeInfo The acceleration structure's size information.
			 * @param type The acceleration structure type.
			 */
			void createAccelerationStructure(const VkAccelerationStructureBuildSizesInfoKHR& sizeInfo, VkAccelerationStructureTypeKHR type);

			/**
			 * Build the created acceleration structure.
			 *
			 * @param sizeInfo The acceleration structure's size information.
			 * @param geometries The geometries to be stored in the acceleration structure.
			 * @param triangleCount The number of triangles in the structure.
			 * @param type The acceleration structure type.
			 */
			void buildAccelerationStructure(const VkAccelerationStructureBuildSizesInfoKHR& sizeInfo, const std::vector<VkAccelerationStructureGeometryKHR>& geometries, uint32_t triangleCount, VkAccelerationStructureTypeKHR type);

		protected:
			VkAccelerationStructureKHR m_AccelerationStructure = VK_NULL_HANDLE;
			VkBuffer m_Buffer = VK_NULL_HANDLE;
			VkDeviceAddress m_DeviceAddress = 0;
			VmaAllocation m_Allocation = nullptr;
		};
	}
}