// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12Image.hpp"
#include "DX12Macros.hpp"

namespace Xenon
{
	namespace Backend
	{
		DX12Image::DX12Image(DX12Device* pDevice, const ImageSpecification& specification, D3D12_HEAP_TYPE heapType /*= D3D12_HEAP_TYPE_DEFAULT*/, D3D12_RESOURCE_STATES resourceStates /*= D3D12_RESOURCE_STATE_COPY_DEST*/)
			: Image(pDevice, specification)
			, m_pDevice(pDevice)
		{
			// Setup the flags.
			D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;
			if (specification.m_Usage & ImageUsage::Graphics) /* Do nothing here. */;
			if (specification.m_Usage & ImageUsage::Storage) /* Do nothing here. */;

			if (specification.m_Usage & ImageUsage::ColorAttachment)
				flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

			if (specification.m_Usage & ImageUsage::DepthAttachment)
				flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

			// Resolve the dimensions.
			D3D12_RESOURCE_DIMENSION dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			switch (specification.m_Type)
			{
			case Xenon::Backend::ImageType::OneDimensional:
				dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
				break;

			case Xenon::Backend::ImageType::TwoDimensional:
			case Xenon::Backend::ImageType::CubeMap:
				break;

			case Xenon::Backend::ImageType::ThreeDImentional:
				dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
				break;

			default:
				XENON_LOG_ERROR("Invalid image type! Defaulting to TwoDimensional");
				break;
			}

			// Setup the rest and create the image.
			D3D12MA::ALLOCATION_DESC allocationDesc = {};
			allocationDesc.HeapType = heapType;

			D3D12_RESOURCE_DESC resourceDescriptor = {};
			resourceDescriptor.MipLevels = 1;	// TODO: Find a better system.
			resourceDescriptor.Format = m_pDevice->convertFormat(specification.m_Format);
			resourceDescriptor.Width = specification.m_Width;
			resourceDescriptor.Height = specification.m_Height;
			resourceDescriptor.DepthOrArraySize = specification.m_Depth;
			resourceDescriptor.Flags = flags;
			resourceDescriptor.SampleDesc.Count = EnumToInt(specification.m_MultiSamplingCount);
			resourceDescriptor.SampleDesc.Quality = 1;	// TODO: Find a better system.
			resourceDescriptor.Dimension = dimension;

			XENON_DX12_ASSERT(pDevice->getAllocator()->CreateResource(
				&allocationDesc,
				&resourceDescriptor,
				/*D3D12_RESOURCE_STATE_COPY_SOURCE | D3D12_RESOURCE_STATE_COPY_DEST | */resourceStates,
				nullptr,
				&m_pAllocation,
				IID_NULL,
				nullptr), "Failed to create the image!");
		}

		DX12Image::~DX12Image()
		{
			try
			{
				m_pDevice->getInstance()->getDeletionQueue().insert([allocation = m_pAllocation] { allocation->Release(); });
			}
			catch (...)
			{
				XENON_DX12_ASSERT(-1, "Failed to push the image deletion function to the deletion queue!");
			}
		}
	}
}