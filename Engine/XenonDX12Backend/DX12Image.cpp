// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12Image.hpp"
#include "DX12Macros.hpp"

namespace Xenon
{
	namespace Backend
	{
		DX12Image::DX12Image(DX12Device* pDevice, const ImageSpecification& specification, D3D12_RESOURCE_STATES resourceStates /*= D3D12_RESOURCE_STATE_COPY_DEST*/, D3D12_HEAP_TYPE heapType /*= D3D12_HEAP_TYPE_DEFAULT*/, D3D12_HEAP_FLAGS heapFlags /*= D3D12_HEAP_FLAG_NONE*/)
			: Image(pDevice, specification)
			, DX12DeviceBoundObject(pDevice)
		{
			// Setup the flags.
			D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;
			if (specification.m_Usage & ImageUsage::Graphics) /* Do nothing here. */;
			if (specification.m_Usage & ImageUsage::Storage) /* Do nothing here. */;

			if (specification.m_Usage & ImageUsage::ColorAttachment)
				flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

			else if (specification.m_Usage & ImageUsage::DepthAttachment)
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
			allocationDesc.ExtraHeapFlags = heapFlags;

			D3D12_RESOURCE_DESC resourceDescriptor = {};
			resourceDescriptor.MipLevels = 1;	XENON_TODO_NOW("(Dhiraj) Find a better system.");
			resourceDescriptor.Width = specification.m_Width;
			resourceDescriptor.Height = specification.m_Height;
			resourceDescriptor.DepthOrArraySize = static_cast<UINT16>(specification.m_Depth);
			resourceDescriptor.Flags = flags;
			resourceDescriptor.SampleDesc.Count = EnumToInt(specification.m_MultiSamplingCount);
			resourceDescriptor.SampleDesc.Quality = 1;	XENON_TODO_NOW("(Dhiraj) Find a better system.");
			resourceDescriptor.Dimension = dimension;

			// Try and create the image using the candidates.
			const auto candidates = GetCandidateFormats(specification.m_Format);
			for (const auto candidate : candidates)
			{
				resourceDescriptor.Format = m_pDevice->convertFormat(candidate);

				// Try and create the image.
				const auto result = pDevice->getAllocator()->CreateResource(
					&allocationDesc,
					&resourceDescriptor,
					resourceStates,
					nullptr,
					&m_pAllocation,
					IID_NULL,
					nullptr);

				// If successful we can return from the loop.
				if (SUCCEEDED(result))
				{
					m_Specification.m_Format = candidate;
					break;
				}
			}

			// Validate if we were able to create an image or not.
			XENON_DX12_ASSERT(m_pAllocation == nullptr ? -1 : S_OK, "Failed to create the image!");
		}

		DX12Image::DX12Image(DX12Image&& other) noexcept
			: Image(std::move(other))
			, DX12DeviceBoundObject(std::move(other))
			, m_pAllocation(std::exchange(other.m_pAllocation, nullptr))
		{
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

		Xenon::Backend::DX12Image& DX12Image::operator=(DX12Image&& other) noexcept
		{
			Image::operator=(std::move(other));
			DX12DeviceBoundObject::operator=(std::move(other));
			m_pAllocation = std::exchange(other.m_pAllocation, nullptr);

			return *this;
		}
	}
}