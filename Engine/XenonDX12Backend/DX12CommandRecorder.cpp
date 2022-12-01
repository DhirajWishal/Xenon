// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12CommandRecorder.hpp"
#include "DX12Macros.hpp"
#include "DX12Buffer.hpp"
#include "DX12Swapchain.hpp"
#include "DX12RasterizingPipeline.hpp"
#include "DX12Descriptor.hpp"

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
}

namespace Xenon
{
	namespace Backend
	{
		DX12CommandRecorder::DX12CommandRecorder(DX12Device* pDevice, CommandRecorderUsage usage, uint32_t bufferCount /*= 1*/)
			: CommandRecorder(pDevice, usage, bufferCount)
			, DX12DeviceBoundObject(pDevice)
			, m_pDevice(pDevice)
		{
			D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			if (usage & CommandRecorderUsage::Secondary)
				type = D3D12_COMMAND_LIST_TYPE_BUNDLE;

			else if (usage & CommandRecorderUsage::Compute)
				type = D3D12_COMMAND_LIST_TYPE_COMPUTE;

			else if (usage & CommandRecorderUsage::Transfer)
				type = D3D12_COMMAND_LIST_TYPE_COPY;

			// Create the command lists.
			m_pCommandLists.reserve(bufferCount);
			for (uint32_t i = 0; i < bufferCount; i++)
			{
				// Create the command allocator.
				ComPtr<ID3D12CommandAllocator> allocator;
				XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateCommandAllocator(type, IID_PPV_ARGS(&allocator)), "Failed to create the command allocator!");

				// Create the command list.
				ComPtr<ID3D12GraphicsCommandList> commandList;
				XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateCommandList(0, type, allocator.Get(), nullptr, IID_PPV_ARGS(&commandList)), "Failed to create the command list!");

				// Create the fence.
				ComPtr<ID3D12Fence> fence;
				XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)), "Failed to create the fence!");

				// Close the command list.
				XENON_DX12_ASSERT(commandList->Close(), "Failed to stop the current command list!");

				// Insert the created objects.
				m_pCommandAllocators.emplace_back(std::move(allocator));
				m_pCommandLists.emplace_back(std::move(commandList));
				m_pCommandListFences.emplace_back(std::move(fence));
			}

			// Select the current objects.
			m_pCurrentCommandAllocator = m_pCommandAllocators[m_CurrentIndex].Get();
			m_pCurrentCommandList = m_pCommandLists[m_CurrentIndex].Get();
			m_pCurrentCommandListFence = m_pCommandListFences[m_CurrentIndex].Get();
		}

		DX12CommandRecorder::~DX12CommandRecorder()
		{
			if (m_IsRecording)
				end();
		}

		void DX12CommandRecorder::begin()
		{
			wait();

			XENON_DX12_ASSERT(m_pCurrentCommandAllocator->Reset(), "Failed to reset the command list allocator!");
			XENON_DX12_ASSERT(m_pCurrentCommandList->Reset(m_pCurrentCommandAllocator, nullptr), "Failed to reset the command list!");
			m_IsRecording = true;
		}

		void DX12CommandRecorder::begin(CommandRecorder* pParent)
		{
			begin();
			pParent->as<DX12CommandRecorder>()->setBundle(m_pCurrentCommandList);
		}

		void DX12CommandRecorder::copy(Buffer* pSource, uint64_t srcOffset, Buffer* pDestination, uint64_t dstOffset, uint64_t size)
		{
			m_pCurrentCommandList->CopyBufferRegion(pDestination->as<DX12Buffer>()->getResource(), dstOffset, pSource->as<DX12Buffer>()->getResource(), srcOffset, size);
		}

		void DX12CommandRecorder::copy(Image* pSource, Swapchain* pDestination)
		{
			// m_pCurrentCommandList->CopyResource(pDestination->as<DX12Swapchain>()->getCurrentSwapchainImageResource(), pSource->as<DX12Image>()->getResource());
		}

		void DX12CommandRecorder::copy(Buffer* pSource, uint64_t bufferOffset, Image* pImage, glm::vec3 imageSize, glm::vec3 imageOffset /*= glm::vec3(0)*/)
		{
			XENON_TODO_NOW("(Dhiraj) Implement this function {}", __FUNCSIG__);
		}

		void DX12CommandRecorder::bind(Rasterizer* pRasterizer, const std::vector<Rasterizer::ClearValueType>& clearValues, bool usingSecondaryCommandRecorders /*= false*/)
		{
			const auto pDxRasterizer = pRasterizer->as<DX12Rasterizer>();
			const auto colorTargetHeapStart = pDxRasterizer->getColorTargetHeapStartCPU();

			if (pDxRasterizer->hasTarget(AttachmentType::Depth | AttachmentType::Stencil))
			{
				const auto depthTargetHeapStart = pDxRasterizer->getDepthTargetHeapStartCPU();
				m_pCurrentCommandList->OMSetRenderTargets(static_cast<UINT>(pDxRasterizer->getColorTargetCount()), &colorTargetHeapStart, TRUE, &depthTargetHeapStart);
				ClearRenderTargets(m_pCurrentCommandList, clearValues, colorTargetHeapStart, pDxRasterizer->getColorTargetDescriptorSize(), depthTargetHeapStart, pDxRasterizer->getDepthTargetDescriptorSize(), pDxRasterizer->getAttachmentTypes());
			}
			else
			{
				m_pCurrentCommandList->OMSetRenderTargets(static_cast<UINT>(pDxRasterizer->getColorTargetCount()), &colorTargetHeapStart, TRUE, nullptr);
				ClearRenderTargets(m_pCurrentCommandList, clearValues, colorTargetHeapStart, pDxRasterizer->getColorTargetDescriptorSize(), {}, 0, pDxRasterizer->getAttachmentTypes());
			}


			m_IsRenderTargetBound = true;
		}

		void DX12CommandRecorder::bind(RasterizingPipeline* pPipeline, const VertexSpecification& vertexSpecification)
		{
			m_pCurrentCommandList->SetGraphicsRootSignature(pPipeline->as<DX12RasterizingPipeline>()->getRootSignature());
			m_pCurrentCommandList->SetPipelineState(pPipeline->as<DX12RasterizingPipeline>()->getPipeline(vertexSpecification).m_PipelineState.Get());
		}

		void DX12CommandRecorder::bind(RasterizingPipeline* pPipeline, Descriptor* pUserDefinedDescriptor, Descriptor* pMaterialDescriptor, Descriptor* pCameraDescriptor)
		{
			const auto& heaps = pPipeline->as<DX12RasterizingPipeline>()->getDescriptorHeapStorage();
			m_pCurrentCommandList->SetDescriptorHeaps(static_cast<UINT>(heaps.size()), heaps.data());

			const auto cbvSrvUavIncementSize = m_pDevice->getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			const auto smaplerIncementSize = m_pDevice->getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

			UINT index = 0;
			if (pUserDefinedDescriptor)
			{
				auto pDx12UserDefinedDescriptor = pUserDefinedDescriptor->as<DX12Descriptor>();
				const auto cbvSrvUavStart = pDx12UserDefinedDescriptor->getCbvSrvUavDescriptorHeapStart();
				const auto samplerStart = pDx12UserDefinedDescriptor->getSamplerescriptorHeapStart();

				if (pDx12UserDefinedDescriptor->hasBuffers())
					m_pCurrentCommandList->SetGraphicsRootDescriptorTable(index++, CD3DX12_GPU_DESCRIPTOR_HANDLE(heaps[0]->GetGPUDescriptorHandleForHeapStart(), cbvSrvUavStart, cbvSrvUavIncementSize));

				if (pDx12UserDefinedDescriptor->hasSampler())
					m_pCurrentCommandList->SetGraphicsRootDescriptorTable(index++, CD3DX12_GPU_DESCRIPTOR_HANDLE(heaps[1]->GetGPUDescriptorHandleForHeapStart(), samplerStart, smaplerIncementSize));
			}

			if (pMaterialDescriptor)
			{
				auto pDx12MaterialDescriptor = pMaterialDescriptor->as<DX12Descriptor>();
				const auto cbvSrvUavStart = pDx12MaterialDescriptor->getCbvSrvUavDescriptorHeapStart();
				const auto samplerStart = pDx12MaterialDescriptor->getSamplerescriptorHeapStart();

				if (pDx12MaterialDescriptor->hasBuffers())
					m_pCurrentCommandList->SetGraphicsRootDescriptorTable(index++, CD3DX12_GPU_DESCRIPTOR_HANDLE(heaps[0]->GetGPUDescriptorHandleForHeapStart(), cbvSrvUavStart, cbvSrvUavIncementSize));

				if (pDx12MaterialDescriptor->hasSampler())
					m_pCurrentCommandList->SetGraphicsRootDescriptorTable(index++, CD3DX12_GPU_DESCRIPTOR_HANDLE(heaps[1]->GetGPUDescriptorHandleForHeapStart(), samplerStart, smaplerIncementSize));
			}

			if (pCameraDescriptor)
			{
				auto pDx12CameraDescriptor = pCameraDescriptor->as<DX12Descriptor>();
				const auto cbvSrvUavStart = pDx12CameraDescriptor->getCbvSrvUavDescriptorHeapStart();
				const auto samplerStart = pDx12CameraDescriptor->getSamplerescriptorHeapStart();

				if (pDx12CameraDescriptor->hasBuffers())
					m_pCurrentCommandList->SetGraphicsRootDescriptorTable(index++, CD3DX12_GPU_DESCRIPTOR_HANDLE(heaps[0]->GetGPUDescriptorHandleForHeapStart(), cbvSrvUavStart, cbvSrvUavIncementSize));

				if (pDx12CameraDescriptor->hasSampler())
					m_pCurrentCommandList->SetGraphicsRootDescriptorTable(index++, CD3DX12_GPU_DESCRIPTOR_HANDLE(heaps[1]->GetGPUDescriptorHandleForHeapStart(), samplerStart, smaplerIncementSize));
			}
		}

		void DX12CommandRecorder::bind(Buffer* pVertexBuffer, uint32_t vertexStride)
		{
			D3D12_VERTEX_BUFFER_VIEW vertexView = {};
			vertexView.BufferLocation = pVertexBuffer->as<DX12Buffer>()->getResource()->GetGPUVirtualAddress();
			vertexView.SizeInBytes = static_cast<UINT>(pVertexBuffer->getSize());
			vertexView.StrideInBytes = vertexStride;
		}

		void DX12CommandRecorder::bind(Buffer* pIndexBuffer, IndexBufferStride indexStride)
		{
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

		void DX12CommandRecorder::drawIndexed(uint64_t vertexOffset, uint64_t indexOffset, uint64_t indexCount, uint32_t instanceCount /*= 1*/, uint32_t firstInstance /*= 0*/)
		{
			m_pCurrentCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_pCurrentCommandList->DrawIndexedInstanced(static_cast<UINT>(indexCount), instanceCount, static_cast<UINT>(indexOffset), static_cast<UINT>(vertexOffset), firstInstance);
		}

		void DX12CommandRecorder::executeChildren()
		{
			if (m_pBundleCommandList != nullptr)
			{
				m_pCurrentCommandList->ExecuteBundle(m_pBundleCommandList);
				m_pBundleCommandList = nullptr;
			}
		}

		void DX12CommandRecorder::end()
		{
			const auto result = m_pCurrentCommandList->Close();

			if (FAILED(result))
			{
				ID3D12InfoQueue* pQueue = nullptr;
				if (SUCCEEDED(m_pDevice->getDevice()->QueryInterface(__uuidof(ID3D12InfoQueue), (void**)&pQueue)))
				{
					const auto count = pQueue->GetNumStoredMessages();

					D3D12_MESSAGE message;
					SIZE_T messageSize;
					pQueue->GetMessageW(count - 1, &message, &messageSize);

					XENON_LOG_ERROR("Failed to stop the current command list! {}", message.pDescription);
				}
				else
				{
					XENON_DX12_ASSERT(result, "Failed to stop the current command list!");
				}
			}

			m_IsRecording = false;
		}

		void DX12CommandRecorder::next()
		{
			const auto index = incrementIndex();
			m_pCurrentCommandAllocator = m_pCommandAllocators[index].Get();
			m_pCurrentCommandList = m_pCommandLists[index].Get();
			m_pCurrentCommandListFence = m_pCommandListFences[index].Get();
		}

		void DX12CommandRecorder::submit(Swapchain* pSawpchain /*= nullptr*/)
		{
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
	}
}