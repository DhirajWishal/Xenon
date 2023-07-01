// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12BottomLevelAccelerationStructure.hpp"
#include "DX12Macros.hpp"
#include "DX12Buffer.hpp"
#include "DX12CommandRecorder.hpp"

namespace /* anonymous */
{
	/**
	 * Get the element format from the component count and the size.
	 *
	 * @param componentCount The number of components.
	 * @param dataType The component data type.
	 * @return The DXGI format.
	 */
	XENON_NODISCARD constexpr DXGI_FORMAT GetElementFormat(uint8_t componentCount, Xenon::Backend::ComponentDataType dataType) noexcept
	{
		if (componentCount == 1)
		{
			switch (dataType)
			{
			case Xenon::Backend::ComponentDataType::Uint8:
				return DXGI_FORMAT_R8_UINT;

			case Xenon::Backend::ComponentDataType::Uint16:
				return DXGI_FORMAT_R16_UINT;

			case Xenon::Backend::ComponentDataType::Uint32:
				return DXGI_FORMAT_R32_UINT;

			case Xenon::Backend::ComponentDataType::Int8:
				return DXGI_FORMAT_R8_SINT;

			case Xenon::Backend::ComponentDataType::Int16:
				return DXGI_FORMAT_R16_SINT;

			case Xenon::Backend::ComponentDataType::Int32:
				return DXGI_FORMAT_R32_SINT;

			case Xenon::Backend::ComponentDataType::Float:
				return DXGI_FORMAT_R32_FLOAT;

			default:
				break;
			}
		}
		else if (componentCount == 2)
		{
			switch (dataType)
			{
			case Xenon::Backend::ComponentDataType::Uint8:
				return DXGI_FORMAT_R8G8_UINT;

			case Xenon::Backend::ComponentDataType::Uint16:
				return DXGI_FORMAT_R16G16_UINT;

			case Xenon::Backend::ComponentDataType::Uint32:
				return DXGI_FORMAT_R32G32_UINT;

			case Xenon::Backend::ComponentDataType::Int8:
				return DXGI_FORMAT_R8G8_SINT;

			case Xenon::Backend::ComponentDataType::Int16:
				return DXGI_FORMAT_R16G16_SINT;

			case Xenon::Backend::ComponentDataType::Int32:
				return DXGI_FORMAT_R32G32_SINT;

			case Xenon::Backend::ComponentDataType::Float:
				return DXGI_FORMAT_R32G32_FLOAT;

			default:
				break;
			}
		}
		else if (componentCount == 3)
		{
			switch (dataType)
			{
			case Xenon::Backend::ComponentDataType::Uint32:
				return DXGI_FORMAT_R32G32B32_UINT;

			case Xenon::Backend::ComponentDataType::Int32:
				return DXGI_FORMAT_R32G32B32_SINT;

			case Xenon::Backend::ComponentDataType::Float:
				return DXGI_FORMAT_R32G32B32_FLOAT;

			default:
				break;
			}
		}
		else if (componentCount == 4)
		{
			switch (dataType)
			{
			case Xenon::Backend::ComponentDataType::Uint8:
				return DXGI_FORMAT_R8G8B8A8_UNORM;

			case Xenon::Backend::ComponentDataType::Uint16:
				return DXGI_FORMAT_R16G16B16A16_UINT;

			case Xenon::Backend::ComponentDataType::Uint32:
				return DXGI_FORMAT_R32G32B32A32_UINT;

			case Xenon::Backend::ComponentDataType::Int8:
				return DXGI_FORMAT_R8G8B8A8_SNORM;

			case Xenon::Backend::ComponentDataType::Int16:
				return DXGI_FORMAT_R16G16B16A16_SINT;

			case Xenon::Backend::ComponentDataType::Int32:
				return DXGI_FORMAT_R32G32B32A32_SINT;

			case Xenon::Backend::ComponentDataType::Float:
				return DXGI_FORMAT_R32G32B32A32_FLOAT;

			default:
				break;
			}
		}

		XENON_LOG_ERROR("There are no available types for the given component count ({}) and component data type ({})!", componentCount, Xenon::EnumToInt(dataType));
		return DXGI_FORMAT_UNKNOWN;
	}
}

namespace Xenon
{
	namespace Backend
	{
		DX12BottomLevelAccelerationStructure::DX12BottomLevelAccelerationStructure(DX12Device* pDevice, const std::vector<AccelerationStructureGeometry>& geometries)
			: BottomLevelAccelerationStructure(pDevice, geometries)
			, DX12AccelerationStructure(pDevice)
		{
			// Setup geometry data.
			std::vector<D3D12_RAYTRACING_GEOMETRY_DESC> geometryDescs;
			geometryDescs.reserve(geometries.size());
			for (const auto& geometry : geometries)
			{
				const auto vertexFormat = GetElementFormat(
					GetAttributeDataTypeComponentCount(geometry.m_VertexSpecification.getElementAttributeDataType(InputElement::VertexPosition)),
					geometry.m_VertexSpecification.getElementComponentDataType(InputElement::VertexPosition)
				);

				const auto vertexSize = geometry.m_VertexSpecification.getSize();

				auto& geometryDesc = geometryDescs.emplace_back();
				geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
				geometryDesc.Triangles.IndexBuffer = geometry.m_pIndexBuffer->as<DX12Buffer>()->getResource()->GetGPUVirtualAddress();
				geometryDesc.Triangles.IndexCount = static_cast<UINT>(geometry.m_pIndexBuffer->getSize()) / EnumToInt(geometry.m_IndexBufferStride);
				geometryDesc.Triangles.IndexFormat = geometry.m_IndexBufferStride == IndexBufferStride::Uint16 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
				geometryDesc.Triangles.VertexFormat = vertexFormat;
				geometryDesc.Triangles.VertexCount = static_cast<UINT>(geometry.m_pVertexBuffer->getSize()) / vertexSize;
				geometryDesc.Triangles.VertexBuffer.StartAddress = geometry.m_pVertexBuffer->as<DX12Buffer>()->getResource()->GetGPUVirtualAddress();
				geometryDesc.Triangles.VertexBuffer.StrideInBytes = vertexSize;
				geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
			}

			// Setup the bottom level structures.
			D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC bottomLevelBuildDesc = {};
			bottomLevelBuildDesc.Inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
			bottomLevelBuildDesc.Inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
			bottomLevelBuildDesc.Inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
			bottomLevelBuildDesc.Inputs.NumDescs = static_cast<UINT>(geometryDescs.size());
			bottomLevelBuildDesc.Inputs.pGeometryDescs = geometryDescs.data();

			D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO bottomLevelPrebuildInfo = {};
			pDevice->getDevice()->GetRaytracingAccelerationStructurePrebuildInfo(&bottomLevelBuildDesc.Inputs, &bottomLevelPrebuildInfo);

			// Validate the pre-build info size.
			if (bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes == 0)
			{
				XENON_LOG_FATAL("The driver does not support acceleration structures!");
				return;
			}

			// Create the scratch buffer.
			createScratchBuffer(bottomLevelPrebuildInfo.ScratchDataSizeInBytes);
			bottomLevelBuildDesc.ScratchAccelerationStructureData = m_pScratchBuffer->GetResource()->GetGPUVirtualAddress();

			// Create the acceleration structure.
			createAccelerationStructure(bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes);
			bottomLevelBuildDesc.DestAccelerationStructureData = m_pAccelerationStructure->GetResource()->GetGPUVirtualAddress();

			// Build the structure.
			auto commandBuffers = DX12CommandRecorder(m_pDevice, CommandRecorderUsage::Graphics);
			commandBuffers.begin();
			commandBuffers.buildAccelerationStructure(bottomLevelBuildDesc);
			commandBuffers.end();
			commandBuffers.submit();
			commandBuffers.wait();
		}
	}
}