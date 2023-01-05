// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12CommandRecorder.hpp"
#include "DX12Macros.hpp"
#include "DX12Buffer.hpp"
#include "DX12Swapchain.hpp"
#include "DX12RasterizingPipeline.hpp"
#include "DX12Descriptor.hpp"
#include "DX12OcclusionQuery.hpp"
#include "DX12RayTracer.hpp"
#include "DX12RayTracingPipeline.hpp"
#include "DX12ShaderBindingTable.hpp"

#include <optick.h>
#include <glm/gtc/type_ptr.hpp>

#ifdef XENON_PLATFORM_WINDOWS
#include <execution> 

#endif // XENON_PLATFORM_WINDOWS

namespace /* anonymous */
{
	/**
	 * Clear the render targets.
	 *
	 * @param pCommandList The command list to record the command to.
	 * @param clearValues The clear values to clear with.
	 * @param colorDescriptorStart The start of the color descriptor.
	 * @param colorDescriptorIncrementSize The color descriptor increment size.
	 * @param depthDescriptorStart The start of the depth descriptor.
	 * @param depthDescriptorIncrementSize The depth descriptor increment size.
	 * @param attachmentTypes The attachment types.
	 */
	void ClearRenderTargets(
		ID3D12GraphicsCommandList* pCommandList,
		const std::vector<Xenon::Backend::Rasterizer::ClearValueType>& clearValues,
		D3D12_CPU_DESCRIPTOR_HANDLE colorDescriptorStart,
		UINT colorDescriptorIncrementSize,
		D3D12_CPU_DESCRIPTOR_HANDLE depthDescriptorStart,
		UINT depthDescriptorIncrementSize,
		Xenon::Backend::AttachmentType attachmentTypes)
	{
		OPTICK_EVENT();

		auto itr = clearValues.begin();
		auto colorDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(colorDescriptorStart);
		auto depthDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(depthDescriptorStart);

		if (attachmentTypes & Xenon::Backend::AttachmentType::Color)
		{
			try
			{
				pCommandList->ClearRenderTargetView(colorDescriptorHandle, glm::value_ptr(std::get<glm::vec4>(*(itr++))), 0, nullptr);
			}
			catch (const std::exception& e)
			{
				XENON_LOG_ERROR("Clear color value error: {}", e.what());
			}

			colorDescriptorHandle.Offset(1, colorDescriptorIncrementSize);
		}

		if (attachmentTypes & Xenon::Backend::AttachmentType::EntityID)
		{
			try
			{
				auto color = glm::vec4(std::get<glm::vec3>(*(itr++)), 0.0f);
				pCommandList->ClearRenderTargetView(colorDescriptorHandle, glm::value_ptr(color), 0, nullptr);
			}
			catch (const std::exception& e)
			{
				XENON_LOG_ERROR("Clear entity ID value error: {}", e.what());
			}

			colorDescriptorHandle.Offset(1, colorDescriptorIncrementSize);
		}

		if (attachmentTypes & Xenon::Backend::AttachmentType::Normal)
		{
			try
			{
				auto color = glm::vec4(std::get<float>(*(itr++)), 0.0f, 0.0f, 0.0f);
				pCommandList->ClearRenderTargetView(colorDescriptorHandle, glm::value_ptr(color), 0, nullptr);
			}
			catch (const std::exception& e)
			{
				XENON_LOG_ERROR("Clear normal value error: {}", e.what());
			}

			colorDescriptorHandle.Offset(1, colorDescriptorIncrementSize);
		}

		if (attachmentTypes & Xenon::Backend::AttachmentType::Depth && attachmentTypes & Xenon::Backend::AttachmentType::Stencil)
		{
			try
			{
				const auto depthValue = std::get<float>(*(itr++));
				const auto stencilValue = std::get<uint32_t>(*(itr++));

				pCommandList->ClearDepthStencilView(depthDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depthValue, static_cast<UINT8>(stencilValue), 0, nullptr);

			}
			catch (const std::exception& e)
			{
				XENON_LOG_ERROR("Clear depth and stencil value error: {}", e.what());
			}

			depthDescriptorHandle.Offset(1, depthDescriptorIncrementSize);
		}

		else if (attachmentTypes & Xenon::Backend::AttachmentType::Depth)
		{
			try
			{
				pCommandList->ClearDepthStencilView(depthDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH, std::get<float>(*(itr++)), 0, 0, nullptr);
			}
			catch (const std::exception& e)
			{
				XENON_LOG_ERROR("Clear depth value error: {}", e.what());
			}

			depthDescriptorHandle.Offset(1, depthDescriptorIncrementSize);
		}

		else if (attachmentTypes & Xenon::Backend::AttachmentType::Stencil)
		{
			try
			{
				pCommandList->ClearDepthStencilView(depthDescriptorHandle, D3D12_CLEAR_FLAG_STENCIL, 1.0f, static_cast<UINT8>(std::get<uint32_t>(*(itr++))), 0, nullptr);
			}
			catch (const std::exception& e)
			{
				XENON_LOG_ERROR("Clear stencil value error: {}", e.what());
			}

			depthDescriptorHandle.Offset(1, depthDescriptorIncrementSize);
		}
	}

	/**
	 * Get the byte size of a format.
	 *
	 * @param format The format to get the size of.
	 * @return The byte size.
	 */
	[[nodiscard]] constexpr uint8_t GetFormatSize(DXGI_FORMAT format) noexcept
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
		DX12CommandRecorder::DX12CommandRecorder(DX12Device* pDevice, CommandRecorderUsage usage, uint32_t bufferCount /*= 1*/)
			: CommandRecorder(pDevice, usage, bufferCount)
			, DX12DeviceBoundObject(pDevice)
		{
			D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			if (usage & CommandRecorderUsage::Secondary)
				type = D3D12_COMMAND_LIST_TYPE_BUNDLE;

			else if (usage & CommandRecorderUsage::Compute)
				type = D3D12_COMMAND_LIST_TYPE_COMPUTE;

			else if (usage & CommandRecorderUsage::Transfer)
				type = D3D12_COMMAND_LIST_TYPE_COPY;

			// Create the command allocator.
			XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateCommandAllocator(type, IID_PPV_ARGS(&m_CommandAllocator)), "Failed to create the command allocator!");
			XENON_DX12_NAME_OBJECT(m_CommandAllocator, "Command Recorder Allocator");

			// Create the command lists.
			m_pCommandLists.reserve(bufferCount);
			for (uint32_t i = 0; i < bufferCount; i++)
			{
				// Create the command list.
				ComPtr<ID3D12GraphicsCommandList5> commandList;
				XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateCommandList(0, type, m_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)), "Failed to create the command list!");

#ifdef XENON_DEBUG
				switch (type)
				{
				case D3D12_COMMAND_LIST_TYPE_DIRECT:
					XENON_DX12_NAME_OBJECT(commandList, "Command Recorder Direct Command List");
					break;

				case D3D12_COMMAND_LIST_TYPE_BUNDLE:
					XENON_DX12_NAME_OBJECT(commandList, "Command Recorder Bundle Command List");
					break;

				case D3D12_COMMAND_LIST_TYPE_COMPUTE:
					XENON_DX12_NAME_OBJECT(commandList, "Command Recorder Compute Command List");
					break;

				case D3D12_COMMAND_LIST_TYPE_COPY:
					XENON_DX12_NAME_OBJECT(commandList, "Command Recorder Copy Command List");
					break;

				case D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE:
					XENON_DX12_NAME_OBJECT(commandList, "Command Recorder Video Decode Command List");
					break;

				case D3D12_COMMAND_LIST_TYPE_VIDEO_PROCESS:
					XENON_DX12_NAME_OBJECT(commandList, "Command Recorder Video Process Command List");
					break;

				case D3D12_COMMAND_LIST_TYPE_VIDEO_ENCODE:
					XENON_DX12_NAME_OBJECT(commandList, "Command Recorder Video Encode Command List");
					break;

				case D3D12_COMMAND_LIST_TYPE_NONE:
					XENON_DX12_NAME_OBJECT(commandList, "Command Recorder None Command List");
					break;

				default:
					XENON_DX12_NAME_OBJECT(commandList, "Command Recorder Unknown Command List");
					break;
				}

#endif // XENON_DEBUG

				// Create the fence.
				ComPtr<ID3D12Fence> fence;
				XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)), "Failed to create the fence!");
				XENON_DX12_NAME_OBJECT(fence, "Command Recorder Fence");

				// Close the command list.
				XENON_DX12_ASSERT(commandList->Close(), "Failed to stop the current command list!");

				// Insert the created objects.
				m_pCommandLists.emplace_back(std::move(commandList));
				m_pCommandListFences.emplace_back(std::move(fence));
			}

			// Select the current objects.
			m_pCurrentCommandList = m_pCommandLists[m_CurrentIndex].Get();
			m_pCurrentCommandListFence = m_pCommandListFences[m_CurrentIndex].Get();
		}

		DX12CommandRecorder::~DX12CommandRecorder()
		{
			try
			{
				if (m_IsRecording)
					end();
			}
			catch (...)
			{
				XENON_LOG_ERROR("Failed to end the command recorder!");
			}
		}

		void DX12CommandRecorder::begin()
		{
			OPTICK_EVENT();

			wait();

			XENON_DX12_ASSERT(m_CommandAllocator->Reset(), "Failed to reset the command list allocator!");
			XENON_DX12_ASSERT(m_pCurrentCommandList->Reset(m_CommandAllocator.Get(), nullptr), "Failed to reset the command list!");
			m_IsRecording = true;
		}

		void DX12CommandRecorder::begin(CommandRecorder* pParent)
		{
			OPTICK_EVENT();

			begin();
			m_pParentCommandRecorder = pParent->as<DX12CommandRecorder>();
			m_pParentCommandRecorder->addBundle(m_pCurrentCommandList);
		}

		void DX12CommandRecorder::copy(Buffer* pSource, uint64_t srcOffset, Buffer* pDestination, uint64_t dstOffset, uint64_t size)
		{
			OPTICK_EVENT();

			m_pCurrentCommandList->CopyBufferRegion(pDestination->as<DX12Buffer>()->getResource(), dstOffset, pSource->as<DX12Buffer>()->getResource(), srcOffset, size);
		}

		void DX12CommandRecorder::copy(Image* pSource, Swapchain* pDestination)
		{
			OPTICK_EVENT();

			auto pDxSource = pSource->as<DX12Image>();
			auto pDxSwapchin = pDestination->as<DX12Swapchain>();
			auto pDestinationResource = pDxSwapchin->getCurrentSwapchainImageResource();

			const auto previousImageState = pDxSource->getCurrentState();

			// Change the destination resource state.
			{
				const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pDestinationResource, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
				m_pCurrentCommandList->ResourceBarrier(1, &barrier);
			}

			// Change the source resource state.
			{
				const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pDxSource->getResource(), previousImageState, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
				m_pCurrentCommandList->ResourceBarrier(1, &barrier);
				pDxSource->setCurrentState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			}

			// Bind the swapchain, scissor and view port.
			auto swpchainHandle = pDxSwapchin->getCPUDescriptorHandle();
			m_pCurrentCommandList->OMSetRenderTargets(1, &swpchainHandle, FALSE, nullptr);

			const D3D12_RECT scissor = CD3DX12_RECT(0, 0, pDxSwapchin->getWindow()->getWidth(), pDxSwapchin->getWindow()->getHeight());
			m_pCurrentCommandList->RSSetScissorRects(1, &scissor);

			const D3D12_VIEWPORT viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(pDxSwapchin->getWindow()->getWidth()), static_cast<float>(pDxSwapchin->getWindow()->getHeight()), 0.0f, 1.0f);
			m_pCurrentCommandList->RSSetViewports(1, &viewport);

			// Prepare the descriptor heap.
			pDxSwapchin->prepareDescriptorForImageCopy(pDxSource);
			const auto& container = pDxSwapchin->getImageToSwapchainCopyContainer();

			// Set the root signature and the pipeline.
			m_pCurrentCommandList->SetGraphicsRootSignature(container.m_RootSignature.Get());
			m_pCurrentCommandList->SetPipelineState(container.m_PipelineState.Get());

			// Bind the descriptor heap.
			const std::array<ID3D12DescriptorHeap*, 2> descriptors = { container.m_CbvSrvUavDescriptorHeap.Get(), container.m_SamplerDescriptorHeap.Get() };
			m_pCurrentCommandList->SetDescriptorHeaps(static_cast<UINT>(descriptors.size()), descriptors.data());
			m_pCurrentCommandList->SetGraphicsRootDescriptorTable(0, descriptors[0]->GetGPUDescriptorHandleForHeapStart());
			m_pCurrentCommandList->SetGraphicsRootDescriptorTable(1, descriptors[1]->GetGPUDescriptorHandleForHeapStart());

			// Bind the vertex buffer and set the primitive topology.
			m_pCurrentCommandList->IASetVertexBuffers(0, 1, &container.m_VertexBufferView);
			m_pCurrentCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// Let's draw!.. By that I mean convert!.. I mean copy!
			m_pCurrentCommandList->DrawInstanced(6, 1, 0, 0);

			// Change the destination resource state.
			{
				const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pDestinationResource, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
				m_pCurrentCommandList->ResourceBarrier(1, &barrier);
			}

			// Change the source resource state.
			{
				const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pDxSource->getResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, previousImageState);
				m_pCurrentCommandList->ResourceBarrier(1, &barrier);
				pDxSource->setCurrentState(previousImageState);
			}
		}

		void DX12CommandRecorder::copy(Buffer* pSource, uint64_t bufferOffset, Image* pImage, glm::vec3 imageSize, glm::vec3 imageOffset /*= glm::vec3(0)*/)
		{
			OPTICK_EVENT();

			const auto pDxImage = pImage->as<DX12Image>();
			const auto pDxBuffer = pSource->as<DX12Buffer>();

			// Change the destination image state.
			if (pDxImage->getCurrentState() != D3D12_RESOURCE_STATE_COPY_DEST)
			{
				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pDxImage->getResource(), pDxImage->getCurrentState(), D3D12_RESOURCE_STATE_COPY_DEST);
				m_pCurrentCommandList->ResourceBarrier(1, &barrier);
			}

			// Change the source buffer state.
			if (pDxBuffer->getResourceState() != D3D12_RESOURCE_STATE_GENERIC_READ)
			{
				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pDxBuffer->getResource(), pDxBuffer->getResourceState(), D3D12_RESOURCE_STATE_COPY_SOURCE);
				m_pCurrentCommandList->ResourceBarrier(1, &barrier);
			}

			// Copy the buffer to the image.
			D3D12_TEXTURE_COPY_LOCATION sourceLocation = {};
			sourceLocation.pResource = pDxBuffer->getResource();
			sourceLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			sourceLocation.PlacedFootprint.Offset = 0;
			sourceLocation.PlacedFootprint.Footprint.Format = m_pDevice->ConvertFormat(pDxImage->getDataFormat());
			sourceLocation.PlacedFootprint.Footprint.Depth = 1;
			sourceLocation.PlacedFootprint.Footprint.Width = pDxImage->getWidth();
			sourceLocation.PlacedFootprint.Footprint.Height = pDxImage->getHeight();
			sourceLocation.PlacedFootprint.Footprint.RowPitch = pDxImage->getWidth() * GetFormatSize(sourceLocation.PlacedFootprint.Footprint.Format);

			D3D12_TEXTURE_COPY_LOCATION destinationLocation = {};
			destinationLocation.pResource = pDxImage->getResource();
			destinationLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			destinationLocation.SubresourceIndex = 0;

			m_pCurrentCommandList->CopyTextureRegion(&destinationLocation, 0, 0, 0, &sourceLocation, nullptr);

			// Change the destination image state.
			if (pDxImage->getUsage() & ImageUsage::Graphics)
			{
				pDxImage->setCurrentState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pDxImage->getResource(), D3D12_RESOURCE_STATE_COPY_DEST, pDxImage->getCurrentState());
				m_pCurrentCommandList->ResourceBarrier(1, &barrier);
			}
			else
			{
				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pDxImage->getResource(), D3D12_RESOURCE_STATE_COPY_DEST, pDxImage->getCurrentState());
				m_pCurrentCommandList->ResourceBarrier(1, &barrier);
			}

			// Change the source buffer state.
			if (pDxBuffer->getResourceState() != D3D12_RESOURCE_STATE_GENERIC_READ)
			{
				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pDxBuffer->getResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, pDxBuffer->getResourceState());
				m_pCurrentCommandList->ResourceBarrier(1, &barrier);
			}
		}

		void DX12CommandRecorder::copy(Image* pSource, const glm::vec3& sourceOffset, Image* pDestination, const glm::vec3& destinationOffset)
		{
			OPTICK_EVENT();

			const auto pDxSourceImage = pSource->as<DX12Image>();
			const auto pDxDestinationImage = pDestination->as<DX12Image>();

			// Change the source image state.
			if (pDxSourceImage->getCurrentState() != D3D12_RESOURCE_STATE_GENERIC_READ)
			{
				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pDxSourceImage->getResource(), pDxSourceImage->getCurrentState(), D3D12_RESOURCE_STATE_COPY_SOURCE);
				m_pCurrentCommandList->ResourceBarrier(1, &barrier);
			}

			// Change the destination image state.
			{
				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pDxDestinationImage->getResource(), pDxDestinationImage->getCurrentState(), D3D12_RESOURCE_STATE_COPY_DEST);
				m_pCurrentCommandList->ResourceBarrier(1, &barrier);
			}

			// Copy the texture region.
			D3D12_TEXTURE_COPY_LOCATION destinationLocation = {};
			destinationLocation.pResource = pDxDestinationImage->getResource();
			destinationLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			destinationLocation.PlacedFootprint.Offset = 0;
			destinationLocation.PlacedFootprint.Footprint.Format = m_pDevice->ConvertFormat(pDxDestinationImage->getDataFormat());
			destinationLocation.PlacedFootprint.Footprint.Depth = 1;
			destinationLocation.PlacedFootprint.Footprint.Width = pDxDestinationImage->getWidth();
			destinationLocation.PlacedFootprint.Footprint.Height = pDxDestinationImage->getHeight();
			destinationLocation.PlacedFootprint.Footprint.RowPitch = pDxDestinationImage->getWidth() * GetFormatSize(destinationLocation.PlacedFootprint.Footprint.Format);

			D3D12_TEXTURE_COPY_LOCATION sourceLocation = {};
			sourceLocation.pResource = pDxSourceImage->getResource();
			sourceLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			sourceLocation.PlacedFootprint.Offset = 0;
			sourceLocation.PlacedFootprint.Footprint.Format = m_pDevice->ConvertFormat(pDxSourceImage->getDataFormat());
			sourceLocation.PlacedFootprint.Footprint.Depth = 1;
			sourceLocation.PlacedFootprint.Footprint.Width = pDxSourceImage->getWidth();
			sourceLocation.PlacedFootprint.Footprint.Height = pDxSourceImage->getHeight();
			sourceLocation.PlacedFootprint.Footprint.RowPitch = pDxSourceImage->getWidth() * GetFormatSize(sourceLocation.PlacedFootprint.Footprint.Format);

			m_pCurrentCommandList->CopyTextureRegion(&destinationLocation, static_cast<UINT>(destinationOffset.x), static_cast<UINT>(destinationOffset.y), static_cast<UINT>(destinationOffset.z), &sourceLocation, nullptr);

			// Change the source image state.
			if (pDxSourceImage->getCurrentState() != D3D12_RESOURCE_STATE_GENERIC_READ)
			{
				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pDxSourceImage->getResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, pDxSourceImage->getCurrentState());
				m_pCurrentCommandList->ResourceBarrier(1, &barrier);
			}

			// Change the destination image state.
			{
				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pDxDestinationImage->getResource(), D3D12_RESOURCE_STATE_COPY_DEST, pDxDestinationImage->getCurrentState());
				m_pCurrentCommandList->ResourceBarrier(1, &barrier);
			}
		}

		void DX12CommandRecorder::resetQuery(OcclusionQuery* pOcclusionQuery)
		{
			OPTICK_EVENT();
		}

		void DX12CommandRecorder::bind(Rasterizer* pRasterizer, const std::vector<Rasterizer::ClearValueType>& clearValues, bool usingSecondaryCommandRecorders /*= false*/)
		{
			OPTICK_EVENT();

			const auto pDxRasterizer = pRasterizer->as<DX12Rasterizer>();
			const auto hasDepthAttachment = pDxRasterizer->hasTarget(AttachmentType::Depth | AttachmentType::Stencil);
			const auto colorAttachmentCount = pDxRasterizer->getColorTargetCount();

			// Setup the color target heap.
			D3D12_CPU_DESCRIPTOR_HANDLE colorTargetHeapStart = {};
			if (colorAttachmentCount > 0)
				colorTargetHeapStart = pDxRasterizer->getColorTargetHeapStartCPU();

			// Setup the depth target heap.
			D3D12_CPU_DESCRIPTOR_HANDLE depthTargetHeapStart = {};
			if (hasDepthAttachment)
				depthTargetHeapStart = pDxRasterizer->getDepthTargetHeapStartCPU();

			// Bind the render targets and clear their value.
			m_pCurrentCommandList->OMSetRenderTargets(static_cast<UINT>(colorAttachmentCount), &colorTargetHeapStart, TRUE, hasDepthAttachment ? &depthTargetHeapStart : nullptr);
			ClearRenderTargets(m_pCurrentCommandList, clearValues, colorTargetHeapStart, pDxRasterizer->getColorTargetDescriptorSize(), depthTargetHeapStart, pDxRasterizer->getDepthTargetDescriptorSize(), pDxRasterizer->getAttachmentTypes());

			m_IsRenderTargetBound = true;
		}

		void DX12CommandRecorder::bind(RasterizingPipeline* pPipeline, const VertexSpecification& vertexSpecification)
		{
			OPTICK_EVENT();

			m_pCurrentCommandList->SetGraphicsRootSignature(pPipeline->as<DX12RasterizingPipeline>()->getRootSignature());
			m_pCurrentCommandList->SetPipelineState(pPipeline->as<DX12RasterizingPipeline>()->getPipeline(vertexSpecification).m_PipelineState.Get());
		}

		void DX12CommandRecorder::bind(RasterizingPipeline* pPipeline, Descriptor* pUserDefinedDescriptor, Descriptor* pMaterialDescriptor, Descriptor* pSceneDescriptor)
		{
			OPTICK_EVENT();

			const auto& heaps = pPipeline->as<DX12RasterizingPipeline>()->getDescriptorHeapStorage();
			m_pCurrentCommandList->SetDescriptorHeaps(static_cast<UINT>(heaps.size()), heaps.data());

			UINT index = 0;
			if (pUserDefinedDescriptor)
			{
				auto pDx12UserDefinedDescriptor = pUserDefinedDescriptor->as<DX12Descriptor>();
				const auto cbvSrvUavStart = pDx12UserDefinedDescriptor->getCbvSrvUavDescriptorHeapStart();
				const auto samplerStart = pDx12UserDefinedDescriptor->getSamplerDescriptorHeapStart();

				if (pDx12UserDefinedDescriptor->hasBuffers())
					m_pCurrentCommandList->SetGraphicsRootDescriptorTable(index++, CD3DX12_GPU_DESCRIPTOR_HANDLE(heaps[0]->GetGPUDescriptorHandleForHeapStart(), cbvSrvUavStart, pDx12UserDefinedDescriptor->getCbvSrvUavDescriptorHeapIncrementSize()));

				if (pDx12UserDefinedDescriptor->hasSampler())
					m_pCurrentCommandList->SetGraphicsRootDescriptorTable(index++, CD3DX12_GPU_DESCRIPTOR_HANDLE(heaps[1]->GetGPUDescriptorHandleForHeapStart(), samplerStart, pDx12UserDefinedDescriptor->getSamplerDescriptorHeapIncrementSize()));
			}

			if (pMaterialDescriptor)
			{
				auto pDx12MaterialDescriptor = pMaterialDescriptor->as<DX12Descriptor>();
				const auto cbvSrvUavStart = pDx12MaterialDescriptor->getCbvSrvUavDescriptorHeapStart();
				const auto samplerStart = pDx12MaterialDescriptor->getSamplerDescriptorHeapStart();

				if (pDx12MaterialDescriptor->hasBuffers())
					m_pCurrentCommandList->SetGraphicsRootDescriptorTable(index++, CD3DX12_GPU_DESCRIPTOR_HANDLE(heaps[0]->GetGPUDescriptorHandleForHeapStart(), cbvSrvUavStart, pDx12MaterialDescriptor->getCbvSrvUavDescriptorHeapIncrementSize()));

				if (pDx12MaterialDescriptor->hasSampler())
					m_pCurrentCommandList->SetGraphicsRootDescriptorTable(index++, CD3DX12_GPU_DESCRIPTOR_HANDLE(heaps[1]->GetGPUDescriptorHandleForHeapStart(), samplerStart, pDx12MaterialDescriptor->getSamplerDescriptorHeapIncrementSize()));
			}

			if (pSceneDescriptor)
			{
				auto pDx12SceneDescriptor = pSceneDescriptor->as<DX12Descriptor>();
				const auto cbvSrvUavStart = pDx12SceneDescriptor->getCbvSrvUavDescriptorHeapStart();
				const auto samplerStart = pDx12SceneDescriptor->getSamplerDescriptorHeapStart();

				if (pDx12SceneDescriptor->hasBuffers())
					m_pCurrentCommandList->SetGraphicsRootDescriptorTable(index++, CD3DX12_GPU_DESCRIPTOR_HANDLE(heaps[0]->GetGPUDescriptorHandleForHeapStart(), cbvSrvUavStart, pDx12SceneDescriptor->getCbvSrvUavDescriptorHeapIncrementSize()));

				if (pDx12SceneDescriptor->hasSampler())
					m_pCurrentCommandList->SetGraphicsRootDescriptorTable(index++, CD3DX12_GPU_DESCRIPTOR_HANDLE(heaps[1]->GetGPUDescriptorHandleForHeapStart(), samplerStart, pDx12SceneDescriptor->getSamplerDescriptorHeapIncrementSize()));
			}
		}

		void DX12CommandRecorder::bind(Buffer* pVertexBuffer, uint32_t vertexStride)
		{
			OPTICK_EVENT();

			D3D12_VERTEX_BUFFER_VIEW vertexView = {};
			vertexView.BufferLocation = pVertexBuffer->as<DX12Buffer>()->getResource()->GetGPUVirtualAddress();
			vertexView.SizeInBytes = static_cast<UINT>(pVertexBuffer->getSize());
			vertexView.StrideInBytes = vertexStride;

			m_pCurrentCommandList->IASetVertexBuffers(0, 1, &vertexView);
		}

		void DX12CommandRecorder::bind(Buffer* pIndexBuffer, IndexBufferStride indexStride)
		{
			OPTICK_EVENT();

			D3D12_INDEX_BUFFER_VIEW indexView = {};
			indexView.BufferLocation = pIndexBuffer->as<DX12Buffer>()->getResource()->GetGPUVirtualAddress();
			indexView.SizeInBytes = static_cast<UINT>(pIndexBuffer->getSize());
			indexView.Format = DXGI_FORMAT_UNKNOWN;

			if (indexStride == IndexBufferStride::Uint16)
				indexView.Format = DXGI_FORMAT_R16_UINT;

			else if (indexStride == IndexBufferStride::Uint32)
				indexView.Format = DXGI_FORMAT_R32_UINT;

			else
				XENON_LOG_ERROR("Invalid index stride!");

			m_pCurrentCommandList->IASetIndexBuffer(&indexView);
		}

		void DX12CommandRecorder::bind(RayTracingPipeline* pPipeline)
		{
			OPTICK_EVENT();

			m_pCurrentCommandList->SetPipelineState1(pPipeline->as<DX12RayTracingPipeline>()->getStateObject());
		}

		void DX12CommandRecorder::bind(RayTracingPipeline* pPipeline, Descriptor* pUserDefinedDescriptor, Descriptor* pMaterialDescriptor, Descriptor* pSceneDescriptor)
		{
			OPTICK_EVENT();

			const auto& heaps = pPipeline->as<DX12RayTracingPipeline>()->getDescriptorHeapStorage();
			m_pCurrentCommandList->SetDescriptorHeaps(static_cast<UINT>(heaps.size()), heaps.data());

			UINT index = 0;
			if (pUserDefinedDescriptor)
			{
				auto pDx12UserDefinedDescriptor = pUserDefinedDescriptor->as<DX12Descriptor>();
				const auto cbvSrvUavStart = pDx12UserDefinedDescriptor->getCbvSrvUavDescriptorHeapStart();
				const auto samplerStart = pDx12UserDefinedDescriptor->getSamplerDescriptorHeapStart();

				if (pDx12UserDefinedDescriptor->hasBuffers())
					m_pCurrentCommandList->SetGraphicsRootDescriptorTable(index++, CD3DX12_GPU_DESCRIPTOR_HANDLE(heaps[0]->GetGPUDescriptorHandleForHeapStart(), cbvSrvUavStart, pDx12UserDefinedDescriptor->getCbvSrvUavDescriptorHeapIncrementSize()));

				if (pDx12UserDefinedDescriptor->hasSampler())
					m_pCurrentCommandList->SetGraphicsRootDescriptorTable(index++, CD3DX12_GPU_DESCRIPTOR_HANDLE(heaps[1]->GetGPUDescriptorHandleForHeapStart(), samplerStart, pDx12UserDefinedDescriptor->getSamplerDescriptorHeapIncrementSize()));
			}

			if (pMaterialDescriptor)
			{
				auto pDx12MaterialDescriptor = pMaterialDescriptor->as<DX12Descriptor>();
				const auto cbvSrvUavStart = pDx12MaterialDescriptor->getCbvSrvUavDescriptorHeapStart();
				const auto samplerStart = pDx12MaterialDescriptor->getSamplerDescriptorHeapStart();

				if (pDx12MaterialDescriptor->hasBuffers())
					m_pCurrentCommandList->SetGraphicsRootDescriptorTable(index++, CD3DX12_GPU_DESCRIPTOR_HANDLE(heaps[0]->GetGPUDescriptorHandleForHeapStart(), cbvSrvUavStart, pDx12MaterialDescriptor->getCbvSrvUavDescriptorHeapIncrementSize()));

				if (pDx12MaterialDescriptor->hasSampler())
					m_pCurrentCommandList->SetGraphicsRootDescriptorTable(index++, CD3DX12_GPU_DESCRIPTOR_HANDLE(heaps[1]->GetGPUDescriptorHandleForHeapStart(), samplerStart, pDx12MaterialDescriptor->getSamplerDescriptorHeapIncrementSize()));
			}

			if (pSceneDescriptor)
			{
				auto pDx12SceneDescriptor = pSceneDescriptor->as<DX12Descriptor>();
				const auto cbvSrvUavStart = pDx12SceneDescriptor->getCbvSrvUavDescriptorHeapStart();
				const auto samplerStart = pDx12SceneDescriptor->getSamplerDescriptorHeapStart();

				if (pDx12SceneDescriptor->hasBuffers())
					m_pCurrentCommandList->SetGraphicsRootDescriptorTable(index++, CD3DX12_GPU_DESCRIPTOR_HANDLE(heaps[0]->GetGPUDescriptorHandleForHeapStart(), cbvSrvUavStart, pDx12SceneDescriptor->getCbvSrvUavDescriptorHeapIncrementSize()));

				if (pDx12SceneDescriptor->hasSampler())
					m_pCurrentCommandList->SetGraphicsRootDescriptorTable(index++, CD3DX12_GPU_DESCRIPTOR_HANDLE(heaps[1]->GetGPUDescriptorHandleForHeapStart(), samplerStart, pDx12SceneDescriptor->getSamplerDescriptorHeapIncrementSize()));
			}
		}

		void DX12CommandRecorder::setViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
		{
			OPTICK_EVENT();

			if (m_Usage & CommandRecorderUsage::Secondary && m_pParentCommandRecorder)
			{
				m_pParentCommandRecorder->setViewport(x, y, width, height, minDepth, maxDepth);
			}
			else
			{
				auto lock = std::scoped_lock(m_Mutex);

				const D3D12_VIEWPORT viewport = CD3DX12_VIEWPORT(x, y, width, height, minDepth, maxDepth);
				m_pCurrentCommandList->RSSetViewports(1, &viewport);
			}
		}

		void DX12CommandRecorder::setViewportNatural(float x, float y, float width, float height, float minDepth, float maxDepth)
		{
			OPTICK_EVENT();

			if (m_Usage & CommandRecorderUsage::Secondary && m_pParentCommandRecorder)
			{
				m_pParentCommandRecorder->setViewportNatural(x, y, width, height, minDepth, maxDepth);
			}
			else
			{
				auto lock = std::scoped_lock(m_Mutex);

				const D3D12_VIEWPORT viewport = CD3DX12_VIEWPORT(x, height - y, width, -height, minDepth, maxDepth);
				m_pCurrentCommandList->RSSetViewports(1, &viewport);
			}
		}

		void DX12CommandRecorder::setScissor(int32_t x, int32_t y, uint32_t width, uint32_t height)
		{
			OPTICK_EVENT();

			if (m_Usage & CommandRecorderUsage::Secondary && m_pParentCommandRecorder)
			{
				m_pParentCommandRecorder->setScissor(x, y, width, height);
			}
			else
			{
				auto lock = std::scoped_lock(m_Mutex);

				const D3D12_RECT scissor = CD3DX12_RECT(x, y, width, height);
				m_pCurrentCommandList->RSSetScissorRects(1, &scissor);
			}
		}

		void DX12CommandRecorder::beginQuery(OcclusionQuery* pOcclusionQuery, uint32_t index)
		{
			OPTICK_EVENT();

			m_pCurrentCommandList->BeginQuery(pOcclusionQuery->as<DX12OcclusionQuery>()->getHeap(), D3D12_QUERY_TYPE_BINARY_OCCLUSION, index);
		}

		void DX12CommandRecorder::drawIndexed(uint64_t vertexOffset, uint64_t indexOffset, uint64_t indexCount, uint32_t instanceCount /*= 1*/, uint32_t firstInstance /*= 0*/)
		{
			OPTICK_EVENT();

			m_pCurrentCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_pCurrentCommandList->DrawIndexedInstanced(static_cast<UINT>(indexCount), instanceCount, static_cast<UINT>(indexOffset), static_cast<UINT>(vertexOffset), firstInstance);
		}

		void DX12CommandRecorder::drawRayTraced(RayTracer* pRayTracer, ShaderBindingTable* pShaderBindingTable)
		{
			OPTICK_EVENT();
			auto pDxBindingTable = pShaderBindingTable->as<DX12ShaderBindingTable>();

			D3D12_DISPATCH_RAYS_DESC desc = {};
			desc.RayGenerationShaderRecord = pDxBindingTable->getRayGenerationAddresRange();
			desc.MissShaderTable = pDxBindingTable->getMissAddressRange();
			desc.HitGroupTable = pDxBindingTable->getHitGroupAddressRange();
			desc.CallableShaderTable = pDxBindingTable->getCallableAddressRange();
			desc.Width = pRayTracer->getCamera()->getWidth();
			desc.Height = pRayTracer->getCamera()->getHeight();
			desc.Depth = 1;

			m_pCurrentCommandList->DispatchRays(&desc);
		}

		void DX12CommandRecorder::endQuery(OcclusionQuery* pOcclusionQuery, uint32_t index)
		{
			OPTICK_EVENT();

			m_pCurrentCommandList->EndQuery(pOcclusionQuery->as<DX12OcclusionQuery>()->getHeap(), D3D12_QUERY_TYPE_BINARY_OCCLUSION, index);
		}

		void DX12CommandRecorder::executeChildren()
		{
			OPTICK_EVENT();

			auto lock = std::scoped_lock(m_Mutex);
			for (const auto pBundleCommandList : m_pBundleCommandLists)
				m_pCurrentCommandList->ExecuteBundle(pBundleCommandList);

			m_pBundleCommandLists.clear();
		}

		void DX12CommandRecorder::getQueryResults(OcclusionQuery* pOcclusionQuery)
		{
			OPTICK_EVENT();

			auto pDxOcclusionQuery = pOcclusionQuery->as<DX12OcclusionQuery>();

			{
				OPTICK_EVENT_DYNAMIC("Resolve Query Data");

				// Copy the occlusion data from the queue to the buffer.
				m_pCurrentCommandList->ResolveQueryData(pDxOcclusionQuery->getHeap(), D3D12_QUERY_TYPE_BINARY_OCCLUSION, 0, static_cast<UINT>(pOcclusionQuery->getSampleCount()), pDxOcclusionQuery->getBuffer(), 0);
			}

			{
				OPTICK_EVENT_DYNAMIC("Copy Query Data");

				// Copy the available data.
				const D3D12_RANGE mapRange = CD3DX12_RANGE(1, 0);

				uint64_t* pSampleData = nullptr;
				XENON_DX12_ASSERT(pDxOcclusionQuery->getBuffer()->Map(0, nullptr, std::bit_cast<void**>(&pSampleData)), "Failed to map the occlusion query buffer!");
				pDxOcclusionQuery->getBuffer()->Unmap(0, &mapRange);

#ifdef XENON_PLATFORM_WINDOWS
				std::copy_n(std::execution::unseq, pSampleData, pDxOcclusionQuery->getSampleCount(), pDxOcclusionQuery->getSamplesPointer());

#else
				std::copy_n(pSampleData, pDxOcclusionQuery->getSampleCount(), pDxOcclusionQuery->getSamplesPointer());

#endif // XENON_PLATFORM_WINDOWS
			}
		}

		void DX12CommandRecorder::buildAccelerationStructure(const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC& desc)
		{
			OPTICK_EVENT();

			m_pCurrentCommandList->BuildRaytracingAccelerationStructure(&desc, 0, nullptr);
		}

		void DX12CommandRecorder::end()
		{
			OPTICK_EVENT();

			XENON_DX12_ASSERT(m_pCurrentCommandList->Close(), "Failed to stop the current command list!");

			m_IsRecording = false;
		}

		void DX12CommandRecorder::next()
		{
			OPTICK_EVENT();

			const auto index = incrementIndex();
			m_pCurrentCommandList = m_pCommandLists[index].Get();
			m_pCurrentCommandListFence = m_pCommandListFences[index].Get();
		}

		void DX12CommandRecorder::submit(Swapchain* pSawpchain /*= nullptr*/)
		{
			OPTICK_EVENT();

			ID3D12CommandQueue* pQueue = m_pDevice->getDirectQueue();
			if (m_Usage & CommandRecorderUsage::Secondary)
				pQueue = m_pDevice->getBundleQueue();

			else if (m_Usage & CommandRecorderUsage::Transfer)
				pQueue = m_pDevice->getCopyQueue();

			std::array<ID3D12CommandList*, 1> pCommandLists = { m_pCurrentCommandList };
			pQueue->ExecuteCommandLists(pCommandLists.size(), pCommandLists.data());
			XENON_DX12_ASSERT(pQueue->Signal(m_pCurrentCommandListFence, 1), "Failed to signal the fence!");
		}

		void DX12CommandRecorder::wait(uint64_t timeout /*= UINT64_MAX*/)
		{
			OPTICK_EVENT();

			const auto nextFence = m_pCurrentCommandListFence->GetCompletedValue() + 1;

			ID3D12CommandQueue* pQueue = m_pDevice->getDirectQueue();
			if (m_Usage & CommandRecorderUsage::Secondary)
				pQueue = m_pDevice->getBundleQueue();

			else if (m_Usage & CommandRecorderUsage::Transfer)
				pQueue = m_pDevice->getCopyQueue();

			XENON_DX12_ASSERT(pQueue->Signal(m_pCurrentCommandListFence, nextFence), "Failed to signal the fence!");

			if (m_pCurrentCommandListFence->GetCompletedValue() < nextFence)
			{
				const auto eventHandle = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);

				// Validate the created event handle.
				if (eventHandle == nullptr)
				{
					XENON_LOG_ERROR("DirectX 12: The created fence event is nullptr!");
					return;
				}

				XENON_DX12_ASSERT(m_pCurrentCommandListFence->SetEventOnCompletion(nextFence, eventHandle), "Failed to set the event completion handle!");
				WaitForSingleObject(eventHandle, static_cast<DWORD>(timeout));
				CloseHandle(eventHandle);
			}
		}

		void DX12CommandRecorder::addBundle(ID3D12GraphicsCommandList* pCommandList)
		{
			OPTICK_EVENT();

			auto lock = std::scoped_lock(m_Mutex);
			m_pBundleCommandLists.emplace_back(pCommandList);
		}
	}
}