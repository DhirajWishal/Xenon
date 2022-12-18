// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12CommandBuffer.hpp"
#include "DX12Macros.hpp"
#include "DX12Buffer.hpp"
#include "DX12Swapchain.hpp"
#include "DX12RasterizingPipeline.hpp"
#include "DX12Descriptor.hpp"

#include <optick.h>
#include <glm/gtc/type_ptr.hpp>

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
		DX12CommandBuffer::DX12CommandBuffer(DX12Device* pDevice, DX12CommandBufferAllocator* pAllocator)
			: CommandBuffer(pDevice, pAllocator)
			, DX12DeviceBoundObject(pDevice)
			, m_pAllocator(pAllocator)
		{
			const auto usage = pAllocator->getUsage();

			D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			if (usage & CommandBufferAllocatorUsage::Secondary)
				type = D3D12_COMMAND_LIST_TYPE_BUNDLE;

			else if (usage & CommandBufferAllocatorUsage::Compute)
				type = D3D12_COMMAND_LIST_TYPE_COMPUTE;

			else if (usage & CommandBufferAllocatorUsage::Transfer)
				type = D3D12_COMMAND_LIST_TYPE_COPY;

			// Create the command list.
			XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateCommandList(0, type, pAllocator->getCommandAllocator(), nullptr, IID_PPV_ARGS(&m_CommandList)), "Failed to create the command list!");

			// Create the fence.
			XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_CommandListFence)), "Failed to create the fence!");
		}

		DX12CommandBuffer::~DX12CommandBuffer()
		{
			if (m_bIsRecording)
			{
				end();
				wait();
			}
		}

		void DX12CommandBuffer::begin()
		{
			OPTICK_EVENT();

			wait();

			XENON_DX12_ASSERT(m_pAllocator->getCommandAllocator()->Reset(), "Failed to reset the command list allocator!");
			XENON_DX12_ASSERT(m_CommandList->Reset(m_pAllocator->getCommandAllocator(), nullptr), "Failed to reset the command list!");
			m_bIsRecording = true;
		}

		void DX12CommandBuffer::begin(CommandBuffer* pParent)
		{
			OPTICK_EVENT();

			begin();
			pParent->as<DX12CommandBuffer>()->addBundle(m_CommandList.Get());
		}

		void DX12CommandBuffer::copy(Buffer* pSource, uint64_t srcOffset, Buffer* pDestination, uint64_t dstOffset, uint64_t size)
		{
			OPTICK_EVENT();

			m_CommandList->CopyBufferRegion(pDestination->as<DX12Buffer>()->getResource(), dstOffset, pSource->as<DX12Buffer>()->getResource(), srcOffset, size);
		}

		void DX12CommandBuffer::copy(Image* pSource, Swapchain* pDestination)
		{
			OPTICK_EVENT();

			auto pDxSource = pSource->as<DX12Image>();
			auto pDxSwapchin = pDestination->as<DX12Swapchain>();
			auto pDestinationResource = pDxSwapchin->getCurrentSwapchainImageResource();

			// Change the destination resource state.
			{
				const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pDestinationResource, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
				m_CommandList->ResourceBarrier(1, &barrier);
			}

			// Change the source resource state.
			{
				const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pDxSource->getResource(), pDxSource->getCurrentState(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
				m_CommandList->ResourceBarrier(1, &barrier);
				pDxSource->setCurrentState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			}

			// Bind the swapchain, scissor and view port.
			auto swpchainHandle = pDxSwapchin->getCPUDescriptorHandle();
			m_CommandList->OMSetRenderTargets(1, &swpchainHandle, FALSE, nullptr);

			const D3D12_RECT scissor = CD3DX12_RECT(0, 0, pDxSwapchin->getWindow()->getWidth(), pDxSwapchin->getWindow()->getHeight());
			m_CommandList->RSSetScissorRects(1, &scissor);

			const D3D12_VIEWPORT viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(pDxSwapchin->getWindow()->getWidth()), static_cast<float>(pDxSwapchin->getWindow()->getHeight()), 0.0f, 1.0f);
			m_CommandList->RSSetViewports(1, &viewport);

			// Prepare the descriptor heap.
			pDxSwapchin->prepareDescriptorForImageCopy(pDxSource);
			const auto& container = pDxSwapchin->getImageToSwapchainCopyContainer();

			// Set the root signature and the pipeline.
			m_CommandList->SetGraphicsRootSignature(container.m_RootSignature.Get());
			m_CommandList->SetPipelineState(container.m_PipelineState.Get());

			// Bind the descriptor heap.
			const auto descriptor = container.m_DescriptorHeap.Get();
			m_CommandList->SetDescriptorHeaps(1, &descriptor);
			m_CommandList->SetGraphicsRootDescriptorTable(0, descriptor->GetGPUDescriptorHandleForHeapStart());

			// Bind the vertex buffer and set the primitive topology.
			m_CommandList->IASetVertexBuffers(0, 1, &container.m_VertexBufferView);
			m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// Let's draw!.. By that I mean convert!.. I mean copy!
			m_CommandList->DrawInstanced(6, 1, 0, 0);

			// Change the destination resource state.
			{
				const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pDestinationResource, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
				m_CommandList->ResourceBarrier(1, &barrier);
			}
		}

		void DX12CommandBuffer::copy(Image* pSource, const glm::vec3& sourceOffset, Image* pDestination, const glm::vec3& destinationOffset)
		{
			OPTICK_EVENT();

			const auto pDxSourceImage = pSource->as<DX12Image>();
			const auto pDxDestinationImage = pDestination->as<DX12Image>();

			// Change the source image state.
			if (pDxSourceImage->getCurrentState() != D3D12_RESOURCE_STATE_GENERIC_READ)
			{
				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pDxSourceImage->getResource(), pDxSourceImage->getCurrentState(), D3D12_RESOURCE_STATE_COPY_SOURCE);
				m_CommandList->ResourceBarrier(1, &barrier);
			}

			// Change the destination image state.
			{
				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pDxDestinationImage->getResource(), pDxDestinationImage->getCurrentState(), D3D12_RESOURCE_STATE_COPY_DEST);
				m_CommandList->ResourceBarrier(1, &barrier);
			}

			// Copy the texture region.
			D3D12_TEXTURE_COPY_LOCATION destinationLocation = {};
			destinationLocation.pResource = pDxDestinationImage->getResource();
			destinationLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			destinationLocation.PlacedFootprint.Offset = 0;
			destinationLocation.PlacedFootprint.Footprint.Format = m_pDevice->convertFormat(pDxDestinationImage->getDataFormat());
			destinationLocation.PlacedFootprint.Footprint.Depth = 1;
			destinationLocation.PlacedFootprint.Footprint.Width = pDxDestinationImage->getWidth();
			destinationLocation.PlacedFootprint.Footprint.Height = pDxDestinationImage->getHeight();
			destinationLocation.PlacedFootprint.Footprint.RowPitch = pDxDestinationImage->getWidth() * GetFormatSize(destinationLocation.PlacedFootprint.Footprint.Format);

			D3D12_TEXTURE_COPY_LOCATION sourceLocation = {};
			sourceLocation.pResource = pDxSourceImage->getResource();
			sourceLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			sourceLocation.PlacedFootprint.Offset = 0;
			sourceLocation.PlacedFootprint.Footprint.Format = m_pDevice->convertFormat(pDxSourceImage->getDataFormat());
			sourceLocation.PlacedFootprint.Footprint.Depth = 1;
			sourceLocation.PlacedFootprint.Footprint.Width = pDxSourceImage->getWidth();
			sourceLocation.PlacedFootprint.Footprint.Height = pDxSourceImage->getHeight();
			sourceLocation.PlacedFootprint.Footprint.RowPitch = pDxSourceImage->getWidth() * GetFormatSize(sourceLocation.PlacedFootprint.Footprint.Format);

			m_CommandList->CopyTextureRegion(&destinationLocation, static_cast<UINT>(destinationOffset.x), static_cast<UINT>(destinationOffset.y), static_cast<UINT>(destinationOffset.z), &sourceLocation, nullptr);

			// Change the source image state.
			if (pDxSourceImage->getCurrentState() != D3D12_RESOURCE_STATE_GENERIC_READ)
			{
				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pDxSourceImage->getResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, pDxSourceImage->getCurrentState());
				m_CommandList->ResourceBarrier(1, &barrier);
			}

			// Change the destination image state.
			{
				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pDxDestinationImage->getResource(), D3D12_RESOURCE_STATE_COPY_DEST, pDxDestinationImage->getCurrentState());
				m_CommandList->ResourceBarrier(1, &barrier);
			}
		}

		void DX12CommandBuffer::copy(Buffer* pSource, uint64_t bufferOffset, Image* pImage, glm::vec3 imageSize, glm::vec3 imageOffset)
		{
			OPTICK_EVENT();

			const auto pDxImage = pImage->as<DX12Image>();
			const auto pDxBuffer = pSource->as<DX12Buffer>();

			// Change the destination image state.
			if (pDxImage->getCurrentState() != D3D12_RESOURCE_STATE_COPY_DEST)
			{
				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pDxImage->getResource(), pDxImage->getCurrentState(), D3D12_RESOURCE_STATE_COPY_DEST);
				m_CommandList->ResourceBarrier(1, &barrier);
			}

			// Change the source buffer state.
			if (pDxBuffer->getResourceState() != D3D12_RESOURCE_STATE_GENERIC_READ)
			{
				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pDxBuffer->getResource(), pDxBuffer->getResourceState(), D3D12_RESOURCE_STATE_COPY_SOURCE);
				m_CommandList->ResourceBarrier(1, &barrier);
			}

			// Copy the buffer to the image.
			D3D12_TEXTURE_COPY_LOCATION sourceLocation = {};
			sourceLocation.pResource = pDxBuffer->getResource();
			sourceLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			sourceLocation.PlacedFootprint.Offset = 0;
			sourceLocation.PlacedFootprint.Footprint.Format = m_pDevice->convertFormat(pDxImage->getDataFormat());
			sourceLocation.PlacedFootprint.Footprint.Depth = 1;
			sourceLocation.PlacedFootprint.Footprint.Width = pDxImage->getWidth();
			sourceLocation.PlacedFootprint.Footprint.Height = pDxImage->getHeight();
			sourceLocation.PlacedFootprint.Footprint.RowPitch = pDxImage->getWidth() * GetFormatSize(sourceLocation.PlacedFootprint.Footprint.Format);

			D3D12_TEXTURE_COPY_LOCATION destinationLocation = {};
			destinationLocation.pResource = pDxImage->getResource();
			destinationLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			destinationLocation.SubresourceIndex = 0;

			m_CommandList->CopyTextureRegion(&destinationLocation, 0, 0, 0, &sourceLocation, nullptr);

			// Change the destination image state.
			if (pDxImage->getUsage() & ImageUsage::Graphics)
			{
				pDxImage->setCurrentState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pDxImage->getResource(), D3D12_RESOURCE_STATE_COPY_DEST, pDxImage->getCurrentState());
				m_CommandList->ResourceBarrier(1, &barrier);
			}
			else
			{
				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pDxImage->getResource(), D3D12_RESOURCE_STATE_COPY_DEST, pDxImage->getCurrentState());
				m_CommandList->ResourceBarrier(1, &barrier);
			}

			// Change the source buffer state.
			if (pDxBuffer->getResourceState() != D3D12_RESOURCE_STATE_GENERIC_READ)
			{
				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pDxBuffer->getResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, pDxBuffer->getResourceState());
				m_CommandList->ResourceBarrier(1, &barrier);
			}
		}

		void DX12CommandBuffer::bind(Rasterizer* pRasterizer, const std::vector<Rasterizer::ClearValueType>& clearValues, bool usingSecondaryCommandRecorders)
		{
			OPTICK_EVENT();

			const auto pDxRasterizer = pRasterizer->as<DX12Rasterizer>();
			const auto hasDepthAttachment = pDxRasterizer->hasTarget(AttachmentType::Depth | AttachmentType::Stencil);
			const auto colorAttachmentCount = pDxRasterizer->getColorTargetCount();

			// Set the proper color image state if needed.
			if (colorAttachmentCount > 0)
			{
				auto& colorTarget = pDxRasterizer->getRenderTargets().front();

				if (colorTarget.getCurrentState() != D3D12_RESOURCE_STATE_RENDER_TARGET)
				{
					auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(colorTarget.getResource(), colorTarget.getCurrentState(), D3D12_RESOURCE_STATE_RENDER_TARGET);
					m_CommandList->ResourceBarrier(1, &barrier);
					colorTarget.setCurrentState(D3D12_RESOURCE_STATE_RENDER_TARGET);
				}
			}

			// Setup the color target heap.
			D3D12_CPU_DESCRIPTOR_HANDLE colorTargetHeapStart = {};
			if (colorAttachmentCount > 0)
				colorTargetHeapStart = pDxRasterizer->getColorTargetHeapStartCPU();

			// Setup the depth target heap.
			D3D12_CPU_DESCRIPTOR_HANDLE depthTargetHeapStart = {};
			if (hasDepthAttachment)
				depthTargetHeapStart = pDxRasterizer->getDepthTargetHeapStartCPU();

			// Bind the render targets and clear their value.
			m_CommandList->OMSetRenderTargets(static_cast<UINT>(colorAttachmentCount), &colorTargetHeapStart, TRUE, hasDepthAttachment ? &depthTargetHeapStart : nullptr);
			ClearRenderTargets(m_CommandList.Get(), clearValues, colorTargetHeapStart, pDxRasterizer->getColorTargetDescriptorSize(), depthTargetHeapStart, pDxRasterizer->getDepthTargetDescriptorSize(), pDxRasterizer->getAttachmentTypes());

			m_bIsRenderTargetBound = true;
		}

		void DX12CommandBuffer::bind(RasterizingPipeline* pPipeline, const VertexSpecification& vertexSpecification)
		{
			OPTICK_EVENT();

			m_CommandList->SetGraphicsRootSignature(pPipeline->as<DX12RasterizingPipeline>()->getRootSignature());
			m_CommandList->SetPipelineState(pPipeline->as<DX12RasterizingPipeline>()->getPipeline(vertexSpecification).m_PipelineState.Get());
		}

		void DX12CommandBuffer::bind(Buffer* pVertexBuffer, uint32_t vertexStride)
		{
			OPTICK_EVENT();

			D3D12_VERTEX_BUFFER_VIEW vertexView = {};
			vertexView.BufferLocation = pVertexBuffer->as<DX12Buffer>()->getResource()->GetGPUVirtualAddress();
			vertexView.SizeInBytes = static_cast<UINT>(pVertexBuffer->getSize());
			vertexView.StrideInBytes = vertexStride;

			m_CommandList->IASetVertexBuffers(0, 1, &vertexView);
		}

		void DX12CommandBuffer::bind(Buffer* pIndexBuffer, IndexBufferStride indexStride)
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

			m_CommandList->IASetIndexBuffer(&indexView);
		}

		void DX12CommandBuffer::bind(RasterizingPipeline* pPipeline, Descriptor* pUserDefinedDescriptor, Descriptor* pMaterialDescriptor, Descriptor* pCameraDescriptor)
		{
			OPTICK_EVENT();

			const auto& heaps = pPipeline->as<DX12RasterizingPipeline>()->getDescriptorHeapStorage();
			m_CommandList->SetDescriptorHeaps(static_cast<UINT>(heaps.size()), heaps.data());

			const auto cbvSrvUavIncementSize = m_pDevice->getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			const auto smaplerIncementSize = m_pDevice->getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

			UINT index = 0;
			if (pUserDefinedDescriptor)
			{
				auto pDx12UserDefinedDescriptor = pUserDefinedDescriptor->as<DX12Descriptor>();
				const auto cbvSrvUavStart = pDx12UserDefinedDescriptor->getCbvSrvUavDescriptorHeapStart();
				const auto samplerStart = pDx12UserDefinedDescriptor->getSamplerescriptorHeapStart();

				if (pDx12UserDefinedDescriptor->hasBuffers())
					m_CommandList->SetGraphicsRootDescriptorTable(index++, CD3DX12_GPU_DESCRIPTOR_HANDLE(heaps[0]->GetGPUDescriptorHandleForHeapStart(), cbvSrvUavStart, cbvSrvUavIncementSize));

				if (pDx12UserDefinedDescriptor->hasSampler())
					m_CommandList->SetGraphicsRootDescriptorTable(index++, CD3DX12_GPU_DESCRIPTOR_HANDLE(heaps[1]->GetGPUDescriptorHandleForHeapStart(), samplerStart, smaplerIncementSize));
			}

			if (pMaterialDescriptor)
			{
				auto pDx12MaterialDescriptor = pMaterialDescriptor->as<DX12Descriptor>();
				const auto cbvSrvUavStart = pDx12MaterialDescriptor->getCbvSrvUavDescriptorHeapStart();
				const auto samplerStart = pDx12MaterialDescriptor->getSamplerescriptorHeapStart();

				if (pDx12MaterialDescriptor->hasBuffers())
					m_CommandList->SetGraphicsRootDescriptorTable(index++, CD3DX12_GPU_DESCRIPTOR_HANDLE(heaps[0]->GetGPUDescriptorHandleForHeapStart(), cbvSrvUavStart, cbvSrvUavIncementSize));

				if (pDx12MaterialDescriptor->hasSampler())
					m_CommandList->SetGraphicsRootDescriptorTable(index++, CD3DX12_GPU_DESCRIPTOR_HANDLE(heaps[1]->GetGPUDescriptorHandleForHeapStart(), samplerStart, smaplerIncementSize));
			}

			if (pCameraDescriptor)
			{
				auto pDx12CameraDescriptor = pCameraDescriptor->as<DX12Descriptor>();
				const auto cbvSrvUavStart = pDx12CameraDescriptor->getCbvSrvUavDescriptorHeapStart();
				const auto samplerStart = pDx12CameraDescriptor->getSamplerescriptorHeapStart();

				if (pDx12CameraDescriptor->hasBuffers())
					m_CommandList->SetGraphicsRootDescriptorTable(index++, CD3DX12_GPU_DESCRIPTOR_HANDLE(heaps[0]->GetGPUDescriptorHandleForHeapStart(), cbvSrvUavStart, cbvSrvUavIncementSize));

				if (pDx12CameraDescriptor->hasSampler())
					m_CommandList->SetGraphicsRootDescriptorTable(index++, CD3DX12_GPU_DESCRIPTOR_HANDLE(heaps[1]->GetGPUDescriptorHandleForHeapStart(), samplerStart, smaplerIncementSize));
			}
		}

		void DX12CommandBuffer::setViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
		{
			const D3D12_VIEWPORT viewport = CD3DX12_VIEWPORT(x, y, width, height, minDepth, maxDepth);
			m_CommandList->RSSetViewports(1, &viewport);
		}

		void DX12CommandBuffer::setViewportNatural(float x, float y, float width, float height, float minDepth, float maxDepth)
		{
			const D3D12_VIEWPORT viewport = CD3DX12_VIEWPORT(x, height - y, width, -height, minDepth, maxDepth);
			m_CommandList->RSSetViewports(1, &viewport);
		}

		void DX12CommandBuffer::setScissor(int32_t x, int32_t y, uint32_t width, uint32_t height)
		{
			const D3D12_RECT scissor = CD3DX12_RECT(x, y, width, height);
			m_CommandList->RSSetScissorRects(1, &scissor);
		}

		void DX12CommandBuffer::drawIndexed(uint64_t vertexOffset, uint64_t indexOffset, uint64_t indexCount, uint32_t instanceCount, uint32_t firstInstance)
		{
			OPTICK_EVENT();

			m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_CommandList->DrawIndexedInstanced(static_cast<UINT>(indexCount), instanceCount, static_cast<UINT>(indexOffset), static_cast<UINT>(vertexOffset), firstInstance);
		}

		void DX12CommandBuffer::executeChildren()
		{
			OPTICK_EVENT();

			for (auto& pCommandList : m_pBundleCommandLists)
				m_CommandList->ExecuteBundle(pCommandList);

			m_pBundleCommandLists.clear();
		}

		void DX12CommandBuffer::end()
		{
			OPTICK_EVENT();

			XENON_DX12_ASSERT(m_CommandList->Close(), "Failed to stop the current command list!");

			m_bIsRecording = false;
		}

		void DX12CommandBuffer::submit(Swapchain* pSwapchain)
		{
			OPTICK_EVENT();

			const auto usage = m_pAllocator->getUsage();

			ID3D12CommandQueue* pQueue = m_pDevice->getDirectQueue();
			if (usage & CommandBufferAllocatorUsage::Secondary)
				pQueue = m_pDevice->getBundleQueue();

			else if (usage & CommandBufferAllocatorUsage::Transfer)
				pQueue = m_pDevice->getCopyQueue();

			std::array<ID3D12CommandList*, 1> pCommandLists = { m_CommandList.Get() };
			pQueue->ExecuteCommandLists(pCommandLists.size(), pCommandLists.data());
			XENON_DX12_ASSERT(pQueue->Signal(m_CommandListFence.Get(), 1), "Failed to signal the fence!");
		}

		void DX12CommandBuffer::wait(uint64_t timeout)
		{
			OPTICK_EVENT();

			const auto nextFence = m_CommandListFence->GetCompletedValue() + 1;
			const auto usage = m_pAllocator->getUsage();

			ID3D12CommandQueue* pQueue = m_pDevice->getDirectQueue();
			if (usage & CommandBufferAllocatorUsage::Secondary)
				pQueue = m_pDevice->getBundleQueue();

			else if (usage & CommandBufferAllocatorUsage::Transfer)
				pQueue = m_pDevice->getCopyQueue();

			XENON_DX12_ASSERT(pQueue->Signal(m_CommandListFence.Get(), nextFence), "Failed to signal the fence!");

			if (m_CommandListFence->GetCompletedValue() < nextFence)
			{
				const auto eventHandle = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);

				// Validate the created event handle.
				if (eventHandle == nullptr)
				{
					XENON_LOG_ERROR("DirectX 12: The created fence event is nullptr!");
					return;
				}

				XENON_DX12_ASSERT(m_CommandListFence->SetEventOnCompletion(nextFence, eventHandle), "Failed to set the event completion handle!");
				WaitForSingleObject(eventHandle, static_cast<DWORD>(timeout));
				CloseHandle(eventHandle);
			}
		}

		void DX12CommandBuffer::addBundle(ID3D12GraphicsCommandList* pCommandList)
		{
			m_pBundleCommandLists.emplace_back(pCommandList);
		}
	}
}