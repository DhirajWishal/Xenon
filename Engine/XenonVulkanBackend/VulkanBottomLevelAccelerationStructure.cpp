// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBottomLevelAccelerationStructure.hpp"
#include "VulkanMacros.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanCommandRecorder.hpp"

namespace /* anonymous */
{
	/**
	 * Get the element format from the component count and the size.
	 *
	 * @param componentCount The number of components.
	 * @param dataType The component data type.
	 * @return The Vulkan format.
	 */
	[[nodiscard]] constexpr VkFormat GetElementFormat(uint8_t componentCount, Xenon::Backend::ComponentDataType dataType) noexcept
	{
		if (componentCount == 1)
		{
			switch (dataType)
			{
			case Xenon::Backend::ComponentDataType::Uint8:
				return VK_FORMAT_R8_UINT;

			case Xenon::Backend::ComponentDataType::Uint16:
				return VK_FORMAT_R16_UINT;

			case Xenon::Backend::ComponentDataType::Uint32:
				return VK_FORMAT_R32_UINT;

			case Xenon::Backend::ComponentDataType::Uint64:
				return VK_FORMAT_R64_UINT;

			case Xenon::Backend::ComponentDataType::Int8:
				return VK_FORMAT_R8_SINT;

			case Xenon::Backend::ComponentDataType::Int16:
				return VK_FORMAT_R16_SINT;

			case Xenon::Backend::ComponentDataType::Int32:
				return VK_FORMAT_R32_SINT;

			case Xenon::Backend::ComponentDataType::Int64:
				return VK_FORMAT_R64_SINT;

			case Xenon::Backend::ComponentDataType::Float:
				return VK_FORMAT_R32_SFLOAT;

			default:
				break;
			}
		}
		else if (componentCount == 2)
		{
			switch (dataType)
			{
			case Xenon::Backend::ComponentDataType::Uint8:
				return VK_FORMAT_R8G8_UINT;

			case Xenon::Backend::ComponentDataType::Uint16:
				return VK_FORMAT_R16G16_UINT;

			case Xenon::Backend::ComponentDataType::Uint32:
				return VK_FORMAT_R32G32_UINT;

			case Xenon::Backend::ComponentDataType::Uint64:
				return VK_FORMAT_R64G64_UINT;

			case Xenon::Backend::ComponentDataType::Int8:
				return VK_FORMAT_R8G8_SINT;

			case Xenon::Backend::ComponentDataType::Int16:
				return VK_FORMAT_R16G16_SINT;

			case Xenon::Backend::ComponentDataType::Int32:
				return VK_FORMAT_R32G32_SINT;

			case Xenon::Backend::ComponentDataType::Int64:
				return VK_FORMAT_R64G64_SINT;

			case Xenon::Backend::ComponentDataType::Float:
				return VK_FORMAT_R32G32_SFLOAT;

			default:
				break;
			}
		}
		else if (componentCount == 3)
		{
			switch (dataType)
			{
			case Xenon::Backend::ComponentDataType::Uint8:
				return VK_FORMAT_R8G8B8_UINT;

			case Xenon::Backend::ComponentDataType::Uint16:
				return VK_FORMAT_R16G16B16_UINT;

			case Xenon::Backend::ComponentDataType::Uint32:
				return VK_FORMAT_R32G32B32_UINT;

			case Xenon::Backend::ComponentDataType::Uint64:
				return VK_FORMAT_R64G64B64_UINT;

			case Xenon::Backend::ComponentDataType::Int8:
				return VK_FORMAT_R8G8B8_SINT;

			case Xenon::Backend::ComponentDataType::Int16:
				return VK_FORMAT_R16G16B16_SINT;

			case Xenon::Backend::ComponentDataType::Int32:
				return VK_FORMAT_R32G32B32_SINT;

			case Xenon::Backend::ComponentDataType::Int64:
				return VK_FORMAT_R64G64B64_SINT;

			case Xenon::Backend::ComponentDataType::Float:
				return VK_FORMAT_R32G32B32_SFLOAT;

			default:
				break;
			}
		}
		else if (componentCount == 4)
		{
			switch (dataType)
			{
			case Xenon::Backend::ComponentDataType::Uint8:
				return VK_FORMAT_R8G8B8A8_UNORM;

			case Xenon::Backend::ComponentDataType::Uint16:
				return VK_FORMAT_R16G16B16A16_UINT;

			case Xenon::Backend::ComponentDataType::Uint32:
				return VK_FORMAT_R32G32B32A32_UINT;

			case Xenon::Backend::ComponentDataType::Uint64:
				return VK_FORMAT_R64G64B64A64_UINT;

			case Xenon::Backend::ComponentDataType::Int8:
				return VK_FORMAT_R8G8B8A8_SNORM;

			case Xenon::Backend::ComponentDataType::Int16:
				return VK_FORMAT_R16G16B16A16_SINT;

			case Xenon::Backend::ComponentDataType::Int32:
				return VK_FORMAT_R32G32B32A32_SINT;

			case Xenon::Backend::ComponentDataType::Int64:
				return VK_FORMAT_R64G64B64A64_SINT;

			case Xenon::Backend::ComponentDataType::Float:
				return VK_FORMAT_R32G32B32A32_SFLOAT;

			default:
				break;
			}
		}

		XENON_LOG_ERROR("There are no available types for the given component count ({}) and component data type ({})!", componentCount, Xenon::EnumToInt(dataType));
		return VK_FORMAT_UNDEFINED;
	}
}

namespace Xenon
{
	namespace Backend
	{
		VulkanBottomLevelAccelerationStructure::VulkanBottomLevelAccelerationStructure(VulkanDevice* pDevice, const std::vector<AccelerationStructureGeometry>& geometries)
			: BottomLevelAccelerationStructure(pDevice, geometries)
			, VulkanAccelerationStructure(pDevice)
		{
			// Setup geometry information.
			std::vector<VkAccelerationStructureGeometryKHR> accelerationStructureGeometries;
			accelerationStructureGeometries.reserve(geometries.size());

			uint32_t triangleCount = 0;
			for (const auto& geometry : geometries)
			{
				triangleCount += static_cast<uint32_t>(geometry.m_pIndexBuffer->getSize() / EnumToInt(geometry.m_IndexBufferStride)) / 3;
				const auto vertexStride = geometry.m_VertexSpecification.getSize();

				const auto vertexFormat = GetElementFormat(
					GetAttributeDataTypeComponentCount(
						geometry.m_VertexSpecification.getElementAttributeDataType(InputElement::VertexPosition)
					),
					geometry.m_VertexSpecification.getElementComponentDataType(InputElement::VertexPosition)
				);

				auto& accelerationStructureGeometry = accelerationStructureGeometries.emplace_back();
				accelerationStructureGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
				accelerationStructureGeometry.pNext = nullptr;
				accelerationStructureGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
				accelerationStructureGeometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
				accelerationStructureGeometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
				accelerationStructureGeometry.geometry.triangles.vertexFormat = vertexFormat;
				accelerationStructureGeometry.geometry.triangles.vertexData.deviceAddress = geometry.m_pVertexBuffer->as<VulkanBuffer>()->getDeviceAddress();
				accelerationStructureGeometry.geometry.triangles.maxVertex = static_cast<uint32_t>(geometry.m_pVertexBuffer->getSize()) / vertexStride;
				accelerationStructureGeometry.geometry.triangles.vertexStride = vertexStride;
				accelerationStructureGeometry.geometry.triangles.indexType = geometry.m_IndexBufferStride == IndexBufferStride::Uint16 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32;
				accelerationStructureGeometry.geometry.triangles.indexData.deviceAddress = geometry.m_pIndexBuffer->as<VulkanBuffer>()->getDeviceAddress();
				accelerationStructureGeometry.geometry.triangles.transformData.deviceAddress = 0;
				accelerationStructureGeometry.geometry.triangles.transformData.hostAddress = nullptr;
			}

			VkAccelerationStructureBuildGeometryInfoKHR accelerationStructureBuildGeometryInfo = {};
			accelerationStructureBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
			accelerationStructureBuildGeometryInfo.pNext = nullptr;
			accelerationStructureBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
			accelerationStructureBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
			accelerationStructureBuildGeometryInfo.geometryCount = static_cast<uint32_t>(accelerationStructureGeometries.size());
			accelerationStructureBuildGeometryInfo.pGeometries = accelerationStructureGeometries.data();

			VkAccelerationStructureBuildSizesInfoKHR accelerationStructureBuildSizesInfo = {};
			accelerationStructureBuildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
			accelerationStructureBuildSizesInfo.pNext = nullptr;

			pDevice->getDeviceTable().vkGetAccelerationStructureBuildSizesKHR(
				pDevice->getLogicalDevice(),
				VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
				&accelerationStructureBuildGeometryInfo,
				&triangleCount,
				&accelerationStructureBuildSizesInfo);

			// Create the acceleration structure.
			createAccelerationStructure(accelerationStructureBuildSizesInfo, VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR);

			// Build the acceleration structure.
			buildAccelerationStructure(accelerationStructureBuildSizesInfo, accelerationStructureGeometries, triangleCount, VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR);
		}
	}
}