// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanAccelerationStructure.hpp"
#include "VulkanMacros.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanCommandRecorder.hpp"

namespace Xenon
{
	namespace Backend
	{
		VulkanAccelerationStructure::VulkanAccelerationStructure(VulkanDevice* pDevice)
			: VulkanDeviceBoundObject(pDevice)
		{
		}

		VulkanAccelerationStructure::~VulkanAccelerationStructure()
		{
			m_pDevice->getDeviceTable().vkDestroyAccelerationStructureKHR(m_pDevice->getLogicalDevice(), m_AccelerationStructure, nullptr);
			m_pDevice->getAllocator().access([this](VmaAllocator allocator) { vmaDestroyBuffer(allocator, m_Buffer, m_Allocation); });
		}

		void VulkanAccelerationStructure::createAccelerationStructure(const VkAccelerationStructureBuildSizesInfoKHR& sizeInfo, VkAccelerationStructureTypeKHR type)
		{
			// Create the buffer.
			VkBufferCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.size = sizeInfo.accelerationStructureSize;
			createInfo.usage = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
			createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;

			VmaAllocationCreateInfo allocationCreateInfo = {};
			allocationCreateInfo.flags = 0;
			allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

			m_pDevice->getAllocator().access([this, createInfo, allocationCreateInfo](VmaAllocator allocator)
				{
					XENON_VK_ASSERT(vmaCreateBuffer(allocator, &createInfo, &allocationCreateInfo, &m_Buffer, &m_Allocation, nullptr), "Failed to create the acceleration structure buffer!");
				}
			);

			// Acceleration structure
			VkAccelerationStructureCreateInfoKHR accelerationStructureCreateInfo = {};
			accelerationStructureCreateInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
			accelerationStructureCreateInfo.buffer = m_Buffer;
			accelerationStructureCreateInfo.size = sizeInfo.accelerationStructureSize;
			accelerationStructureCreateInfo.type = type;
			XENON_VK_ASSERT(m_pDevice->getDeviceTable().vkCreateAccelerationStructureKHR(m_pDevice->getLogicalDevice(), &accelerationStructureCreateInfo, nullptr, &m_AccelerationStructure), "Failed to create the acceleration structure!");

			// AS device address
			VkAccelerationStructureDeviceAddressInfoKHR accelerationDeviceAddressInfo = {};
			accelerationDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
			accelerationDeviceAddressInfo.accelerationStructure = m_AccelerationStructure;
			m_DeviceAddress = m_pDevice->getDeviceTable().vkGetAccelerationStructureDeviceAddressKHR(m_pDevice->getLogicalDevice(), &accelerationDeviceAddressInfo);
		}

		void VulkanAccelerationStructure::buildAccelerationStructure(const VkAccelerationStructureBuildSizesInfoKHR& sizeInfo, const std::vector<VkAccelerationStructureGeometryKHR>& geometries, uint32_t triangleCount, VkAccelerationStructureTypeKHR type)
		{
			auto scratchBuffer = VulkanBuffer(m_pDevice, sizeInfo.buildScratchSize, BufferType::Scratch);

			VkAccelerationStructureBuildGeometryInfoKHR accelerationBuildGeometryInfo = {};
			accelerationBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
			accelerationBuildGeometryInfo.pNext = nullptr;
			accelerationBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
			accelerationBuildGeometryInfo.type = type;
			accelerationBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
			accelerationBuildGeometryInfo.dstAccelerationStructure = m_AccelerationStructure;
			accelerationBuildGeometryInfo.geometryCount = static_cast<uint32_t>(geometries.size());
			accelerationBuildGeometryInfo.pGeometries = geometries.data();
			accelerationBuildGeometryInfo.scratchData.deviceAddress = scratchBuffer.getDeviceAddress();

			VkAccelerationStructureBuildRangeInfoKHR accelerationStructureBuildRangeInfo = {};
			accelerationStructureBuildRangeInfo.primitiveCount = triangleCount;
			accelerationStructureBuildRangeInfo.primitiveOffset = 0;
			accelerationStructureBuildRangeInfo.firstVertex = 0;
			accelerationStructureBuildRangeInfo.transformOffset = 0;

			auto commandBuffers = VulkanCommandRecorder(m_pDevice, CommandRecorderUsage::Transfer);
			commandBuffers.begin();
			commandBuffers.buildAccelerationStructure(accelerationBuildGeometryInfo, { &accelerationStructureBuildRangeInfo });
			commandBuffers.end();
			commandBuffers.submit();
			commandBuffers.wait();
		}
	}
}