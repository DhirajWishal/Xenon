// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12Image.hpp"
#include "DX12Macros.hpp"
#include "DX12Buffer.hpp"

#include <optick.h>

namespace /* anonymous */
{
	/**
	 * Get the byte size of a format.
	 *
	 * @param format The format to get the size of.
	 * @return The byte size.
	 */
	XENON_NODISCARD constexpr uint8_t GetFormatSize(DXGI_FORMAT format) noexcept
	{
		switch (format)
		{
		case DXGI_FORMAT_UNKNOWN:
			return 0;

		case DXGI_FORMAT_R8_SINT:
			return sizeof(int8_t);

		case DXGI_FORMAT_R8G8_SINT:
			return sizeof(int8_t[2]);

		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			return sizeof(uint8_t[4]);

		case DXGI_FORMAT_R8_UNORM:
			return sizeof(uint8_t);

		case DXGI_FORMAT_R8G8_UNORM:
			return sizeof(uint8_t[2]);

		case DXGI_FORMAT_R8G8B8A8_UNORM:
			return sizeof(uint8_t[4]);

		case DXGI_FORMAT_B8G8R8A8_UNORM:
			return sizeof(uint8_t[4]);

		case DXGI_FORMAT_R16_FLOAT:
			return sizeof(int16_t);

		case DXGI_FORMAT_R16G16_FLOAT:
			return sizeof(int16_t[2]);

		case DXGI_FORMAT_R16G16B16A16_FLOAT:
			return sizeof(int16_t[4]);

		case DXGI_FORMAT_R32_FLOAT:
			return sizeof(float);

		case DXGI_FORMAT_R32G32_FLOAT:
			return sizeof(float[2]);

		case DXGI_FORMAT_R32G32B32_FLOAT:
			return sizeof(float[3]);

		case DXGI_FORMAT_R32G32B32A32_FLOAT:
			return sizeof(float[4]);

		case DXGI_FORMAT_D16_UNORM:
			return sizeof(uint16_t);

		case DXGI_FORMAT_D32_FLOAT:
			return sizeof(float);

		case DXGI_FORMAT_D24_UNORM_S8_UINT:
			return (24 / 8) + sizeof(int8_t);

		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
			return 1 + (24 / 8) + sizeof(float);

		default:
			XENON_LOG_ERROR("Invalid or unsupported data format! Defaulting to Undefined.");
			return 0;
		}
	}
}

namespace Xenon
{
	namespace Backend
	{
		DX12Image::DX12Image(DX12Device* pDevice, const ImageSpecification& specification, D3D12_RESOURCE_STATES resourceStates /*= D3D12_RESOURCE_STATE_COPY_DEST*/, D3D12_HEAP_TYPE heapType /*= D3D12_HEAP_TYPE_DEFAULT*/, D3D12_HEAP_FLAGS heapFlags /*= D3D12_HEAP_FLAG_NONE*/, D3D12_CLEAR_VALUE* pClearValue /*= nullptr*/)
			: Image(pDevice, specification)
			, DX12DeviceBoundObject(pDevice)
			, m_CurrentState(resourceStates)
		{
			// Setup the flags.
			D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;
			if (specification.m_Usage & ImageUsage::Graphics)
			{
				const auto formatSize = GetFormatSize(DX12Device::ConvertFormat(specification.m_Format));
				const auto dataPitch = static_cast<uint32_t>(std::ceil(static_cast<float>(getWidth() * formatSize) / D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) * D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
				m_Specification.m_Width = dataPitch / formatSize;
			}

			if (specification.m_Usage & ImageUsage::Storage) NoOp();

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

			case Xenon::Backend::ImageType::ThreeDimensional:
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
			resourceDescriptor.MipLevels = 1;
			resourceDescriptor.Width = m_Specification.m_Width;
			resourceDescriptor.Height = m_Specification.m_Height;
			resourceDescriptor.DepthOrArraySize = static_cast<UINT16>(m_Specification.m_Depth);
			resourceDescriptor.Flags = flags;
			resourceDescriptor.SampleDesc.Count = EnumToInt(m_Specification.m_MultiSamplingCount);
			resourceDescriptor.Dimension = dimension;

			// Try and create the image using the candidates.
			for (const auto candidate : GetCandidateFormats(m_Specification.m_Format))
			{
				resourceDescriptor.Format = DX12Device::ConvertFormat(candidate);

				// Get the best multisample quality level.
				D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msLevels = {};
				msLevels.Format = resourceDescriptor.Format;
				msLevels.SampleCount = resourceDescriptor.SampleDesc.Count;
				msLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;

				XENON_DX12_ASSERT(m_pDevice->getDevice()->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msLevels, sizeof(msLevels)), "Failed to get the best multisample quality level!");
				resourceDescriptor.SampleDesc.Count = msLevels.SampleCount;
				resourceDescriptor.SampleDesc.Quality = msLevels.NumQualityLevels > 0 ? msLevels.NumQualityLevels - 1 : 0;

				// Try and create the image.
				const auto result = pDevice->getAllocator()->CreateResource(
					&allocationDesc,
					&resourceDescriptor,
					resourceStates,
					pClearValue,
					&m_pAllocation,
					IID_NULL,
					nullptr
				);

				// If successful we can return from the loop.
				if (SUCCEEDED(result))
				{
					m_Specification.m_Format = candidate;
					m_Specification.m_MultiSamplingCount = static_cast<MultiSamplingCount>(resourceDescriptor.SampleDesc.Count);
					m_SampleDesc = resourceDescriptor.SampleDesc;
					break;
				}
			}

			// Validate if we were able to create an image or not.
			XENON_DX12_ASSERT(m_pAllocation == nullptr ? -1 : S_OK, "Failed to create the image!");
			XENON_DX12_NAME_OBJECT(getResource(), "Image");

			// Create the allocator.
			XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator)), "Failed to create the copy command allocator!");
			XENON_DX12_NAME_OBJECT(m_CommandAllocator, "Image Command Allocator");

			// Create the command list.
			XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_CommandList)), "Failed to create the copy command list!");
			XENON_DX12_NAME_OBJECT(m_CommandList, "Image Command List");

			// End the command list.
			XENON_DX12_ASSERT(m_CommandList->Close(), "Failed to stop the current command list!");
		}

		DX12Image::DX12Image(DX12Image&& other) noexcept
			: Image(std::move(other))
			, DX12DeviceBoundObject(std::move(other))
			, m_pAllocation(std::exchange(other.m_pAllocation, nullptr))
			, m_CurrentState(std::exchange(other.m_CurrentState, D3D12_RESOURCE_STATE_COMMON))
		{
		}

		DX12Image::~DX12Image()
		{
			if (m_pDevice)
				m_pAllocation->Release();
		}

		void DX12Image::copyFrom(Buffer* pSrcBuffer, CommandRecorder* pCommandRecorder /*= nullptr*/)
		{
			OPTICK_EVENT();

			auto pSourceBuffer = pSrcBuffer->as<DX12Buffer>();

			// Begin the command list.
			XENON_DX12_ASSERT(m_CommandAllocator->Reset(), "Failed to reset the current command allocator!");
			XENON_DX12_ASSERT(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr), "Failed to reset the current command list!");

			// Set the proper resource states.
			// Destination (this)
			if (m_CurrentState != D3D12_RESOURCE_STATE_COPY_DEST)
			{
				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pAllocation->GetResource(), m_CurrentState, D3D12_RESOURCE_STATE_COPY_DEST);
				m_CommandList->ResourceBarrier(1, &barrier);
			}

			// Source
			if (pSourceBuffer->getResourceState() != D3D12_RESOURCE_STATE_GENERIC_READ)
			{
				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pSourceBuffer->getResource(), pSourceBuffer->getResourceState(), D3D12_RESOURCE_STATE_COPY_SOURCE);
				m_CommandList->ResourceBarrier(1, &barrier);
			}

			// Copy the buffer to the image.
			D3D12_TEXTURE_COPY_LOCATION sourceLocation = {};
			sourceLocation.pResource = pSourceBuffer->getResource();
			sourceLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			sourceLocation.PlacedFootprint.Offset = 0;
			sourceLocation.PlacedFootprint.Footprint.Format = DX12Device::ConvertFormat(m_Specification.m_Format);
			sourceLocation.PlacedFootprint.Footprint.Depth = 1;
			sourceLocation.PlacedFootprint.Footprint.Width = getWidth();
			sourceLocation.PlacedFootprint.Footprint.Height = getHeight();
			sourceLocation.PlacedFootprint.Footprint.RowPitch = getWidth() * GetFormatSize(sourceLocation.PlacedFootprint.Footprint.Format);

			D3D12_TEXTURE_COPY_LOCATION destinationLocation = {};
			destinationLocation.pResource = getResource();
			destinationLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			destinationLocation.SubresourceIndex = 0;

			m_CommandList->CopyTextureRegion(&destinationLocation, 0, 0, 0, &sourceLocation, nullptr);

			// Change the state back to previous.
			// Destination (this)
			if (getUsage() & ImageUsage::Graphics)
			{
				m_CurrentState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pAllocation->GetResource(), D3D12_RESOURCE_STATE_COPY_DEST, m_CurrentState);
				m_CommandList->ResourceBarrier(1, &barrier);
			}
			else
			{
				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pAllocation->GetResource(), D3D12_RESOURCE_STATE_COPY_DEST, m_CurrentState);
				m_CommandList->ResourceBarrier(1, &barrier);
			}

			// Source
			if (pSourceBuffer->getResourceState() != D3D12_RESOURCE_STATE_GENERIC_READ)
			{
				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pSourceBuffer->getResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, pSourceBuffer->getResourceState());
				m_CommandList->ResourceBarrier(1, &barrier);
			}

			// End the command list.
			XENON_DX12_ASSERT(m_CommandList->Close(), "Failed to stop the current command list!");

			// Submit the command list to be executed.
			std::array<ID3D12CommandList*, 1> ppCommandLists = { m_CommandList.Get() };
			m_pDevice->getDirectQueue()->ExecuteCommandLists(ppCommandLists.size(), ppCommandLists.data());

			// Wait till the command is done executing.
			ComPtr<ID3D12Fence> fence;
			XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)), "Failed to create the fence!");
			XENON_DX12_NAME_OBJECT(fence, "Image Buffer Copy Fence");

			XENON_DX12_ASSERT(m_pDevice->getDirectQueue()->Signal(fence.Get(), 1), "Failed to signal the fence!");

			// Setup synchronization.
			auto fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

			// Validate the event.
			if (fenceEvent == nullptr)
			{
				XENON_LOG_ERROR("Failed to wait till the command list execution!");
				return;
			}

			// Set the event and wait.
			XENON_DX12_ASSERT(fence->SetEventOnCompletion(1, fenceEvent), "Failed to set the fence event on completion event!");
			WaitForSingleObjectEx(fenceEvent, std::numeric_limits<DWORD>::max(), FALSE);
			CloseHandle(fenceEvent);
		}

		void DX12Image::copyFrom(Image* pSrcImage, CommandRecorder* pCommandRecorder /*= nullptr*/)
		{
			OPTICK_EVENT();

			auto pSourceImage = pSrcImage->as<DX12Image>();

			// Begin the command list.
			XENON_DX12_ASSERT(m_CommandAllocator->Reset(), "Failed to reset the current command allocator!");
			XENON_DX12_ASSERT(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr), "Failed to reset the current command list!");

			// Set the proper resource states.
			// Destination (this)
			if (m_CurrentState != D3D12_RESOURCE_STATE_COPY_DEST)
			{
				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pAllocation->GetResource(), m_CurrentState, D3D12_RESOURCE_STATE_COPY_DEST);
				m_CommandList->ResourceBarrier(1, &barrier);
			}

			// Source
			if (pSourceImage->getCurrentState() != D3D12_RESOURCE_STATE_GENERIC_READ)
			{
				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pSourceImage->getResource(), pSourceImage->getCurrentState(), D3D12_RESOURCE_STATE_COPY_SOURCE);
				m_CommandList->ResourceBarrier(1, &barrier);
			}

			// Copy the buffer to the image.
			D3D12_TEXTURE_COPY_LOCATION sourceLocation = CD3DX12_TEXTURE_COPY_LOCATION(pSourceImage->getResource(), D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT);
			sourceLocation.PlacedFootprint.Offset = 0;
			sourceLocation.PlacedFootprint.Footprint.Format = DX12Device::ConvertFormat(pSrcImage->getDataFormat());
			sourceLocation.PlacedFootprint.Footprint.Depth = pSrcImage->getDepth();
			sourceLocation.PlacedFootprint.Footprint.Width = pSrcImage->getWidth();
			sourceLocation.PlacedFootprint.Footprint.Height = pSrcImage->getHeight();
			sourceLocation.PlacedFootprint.Footprint.RowPitch = pSrcImage->getWidth() * GetFormatSize(sourceLocation.PlacedFootprint.Footprint.Format);

			D3D12_TEXTURE_COPY_LOCATION destinationLocation = CD3DX12_TEXTURE_COPY_LOCATION(getResource(), D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT);
			destinationLocation.PlacedFootprint.Offset = 0;
			destinationLocation.PlacedFootprint.Footprint.Format = DX12Device::ConvertFormat(getDataFormat());
			destinationLocation.PlacedFootprint.Footprint.Depth = getDepth();
			destinationLocation.PlacedFootprint.Footprint.Width = getWidth();
			destinationLocation.PlacedFootprint.Footprint.Height = getHeight();
			destinationLocation.PlacedFootprint.Footprint.RowPitch = getWidth() * GetFormatSize(destinationLocation.PlacedFootprint.Footprint.Format);

			m_CommandList->CopyTextureRegion(&destinationLocation, 0, 0, 0, &sourceLocation, nullptr);

			// Change the state back to previous.
			// Destination (this)
			if (getUsage() & ImageUsage::Graphics)
			{
				m_CurrentState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pAllocation->GetResource(), D3D12_RESOURCE_STATE_COPY_DEST, m_CurrentState);
				m_CommandList->ResourceBarrier(1, &barrier);
			}
			else
			{
				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pAllocation->GetResource(), D3D12_RESOURCE_STATE_COPY_DEST, m_CurrentState);
				m_CommandList->ResourceBarrier(1, &barrier);
			}

			// Source
			if (pSourceImage->getCurrentState() != D3D12_RESOURCE_STATE_GENERIC_READ)
			{
				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pSourceImage->getResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, pSourceImage->getCurrentState());
				m_CommandList->ResourceBarrier(1, &barrier);
			}

			// End the command list.
			XENON_DX12_ASSERT(m_CommandList->Close(), "Failed to stop the current command list!");

			// Submit the command list to be executed.
			std::array<ID3D12CommandList*, 1> ppCommandLists = { m_CommandList.Get() };
			m_pDevice->getDirectQueue()->ExecuteCommandLists(ppCommandLists.size(), ppCommandLists.data());

			// Wait till the command is done executing.
			ComPtr<ID3D12Fence> fence;
			XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)), "Failed to create the fence!");
			XENON_DX12_NAME_OBJECT(fence, "Image Image Copy Fence");

			XENON_DX12_ASSERT(m_pDevice->getDirectQueue()->Signal(fence.Get(), 1), "Failed to signal the fence!");

			// Setup synchronization.
			auto fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

			// Validate the event.
			if (fenceEvent == nullptr)
			{
				XENON_LOG_ERROR("Failed to wait till the command list execution!");
				return;
			}

			// Set the event and wait.
			XENON_DX12_ASSERT(fence->SetEventOnCompletion(1, fenceEvent), "Failed to set the fence event on completion event!");
			WaitForSingleObjectEx(fenceEvent, std::numeric_limits<DWORD>::max(), FALSE);
			CloseHandle(fenceEvent);
		}

		void DX12Image::generateMipMaps(CommandRecorder* pCommandRecorder /*= nullptr*/)
		{
			OPTICK_EVENT();
		}

		Xenon::Backend::DX12Image& DX12Image::operator=(DX12Image&& other) noexcept
		{
			Image::operator=(std::move(other));
			DX12DeviceBoundObject::operator=(std::move(other));
			m_pAllocation = std::exchange(other.m_pAllocation, nullptr);
			m_CurrentState = std::exchange(other.m_CurrentState, D3D12_RESOURCE_STATE_COMMON);

			return *this;
		}
	}
}