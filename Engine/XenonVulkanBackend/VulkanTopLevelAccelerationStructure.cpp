// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#include "VulkanTopLevelAccelerationStructure.hpp"
#include "VulkanMacros.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanBottomLevelAccelerationStructure.hpp"

namespace Xenon
{
	namespace Backend
	{
		VulkanTopLevelAccelerationStructure::VulkanTopLevelAccelerationStructure(VulkanDevice* pDevice, const std::vector<BottomLevelAccelerationStructure*>& pBottomLevelAccelerationStructures)
			: TopLevelAccelerationStructure(pDevice, pBottomLevelAccelerationStructures)
			, VulkanAccelerationStructure(pDevice)
		{
			// Build the instances.
			std::vector<VkAccelerationStructureInstanceKHR> instances;
			instances.reserve(pBottomLevelAccelerationStructures.size());

			for (const auto pAccelerationStructure : pBottomLevelAccelerationStructures)
			{
				const VkTransformMatrixKHR transformMatrix = {
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f
				};

				auto& instance = instances.emplace_back();
				instance.transform = transformMatrix;
				instance.instanceCustomIndex = 0;
				instance.mask = 0xFF;
				instance.instanceShaderBindingTableRecordOffset = 0;
				instance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
				instance.accelerationStructureReference = pAccelerationStructure->as<VulkanBottomLevelAccelerationStructure>()->getDeviceAddress();
			}

			const auto instanceDataSize = sizeof(VkAccelerationStructureInstanceKHR) * instances.size();

			// Create the instance buffer to store the instance data.
			auto instanceBuffer = VulkanBuffer(pDevice, instanceDataSize, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST);
			instanceBuffer.write(ToBytes(instances.data()), instanceDataSize);

			// Setup the acceleration structure information.
			VkAccelerationStructureGeometryKHR accelerationStructureGeometry = {};
			accelerationStructureGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
			accelerationStructureGeometry.pNext = nullptr;
			accelerationStructureGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
			accelerationStructureGeometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
			accelerationStructureGeometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
			accelerationStructureGeometry.geometry.instances.arrayOfPointers = VK_FALSE;
			accelerationStructureGeometry.geometry.instances.data.deviceAddress = instanceBuffer.getDeviceAddress();

			VkAccelerationStructureBuildGeometryInfoKHR accelerationStructureBuildGeometryInfo = {};
			accelerationStructureBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
			accelerationStructureBuildGeometryInfo.pNext = nullptr;
			accelerationStructureBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
			accelerationStructureBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
			accelerationStructureBuildGeometryInfo.geometryCount = 1;
			accelerationStructureBuildGeometryInfo.pGeometries = &accelerationStructureGeometry;

			const auto instanceCount = static_cast<uint32_t>(instances.size());

			// Get the acceleration structure sizes.
			VkAccelerationStructureBuildSizesInfoKHR accelerationStructureBuildSizesInfo = {};
			accelerationStructureBuildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
			accelerationStructureBuildSizesInfo.pNext = nullptr;

			pDevice->getDeviceTable().vkGetAccelerationStructureBuildSizesKHR(
				pDevice->getLogicalDevice(),
				VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
				&accelerationStructureBuildGeometryInfo,
				&instanceCount,
				&accelerationStructureBuildSizesInfo);

			// Create the acceleration structure.
			createAccelerationStructure(accelerationStructureBuildSizesInfo, VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR);

			// Build the acceleration structure.
			buildAccelerationStructure(accelerationStructureBuildSizesInfo, { accelerationStructureGeometry }, instanceCount, VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR);
		}
	}
}