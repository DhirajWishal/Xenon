// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12CommandRecorder.hpp"
#include "DX12Macros.hpp"
#include "DX12Buffer.hpp"
#include "DX12Swapchain.hpp"
#include "DX12RasterizingPipeline.hpp"
#include "DX12Descriptor.hpp"

#include <glm/gtc/type_ptr.hpp>

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

		void DX12CommandRecorder::begin()
		{
			wait();

			XENON_DX12_ASSERT(m_pCurrentCommandAllocator->Reset(), "Failed to reset the command list allocator!");
			XENON_DX12_ASSERT(m_pCurrentCommandList->Reset(m_pCurrentCommandAllocator, nullptr), "Failed to reset the command list!");
		}

		void DX12CommandRecorder::begin(CommandRecorder* pParent)
		{
			XENON_TODO_NOW("(Dhiraj) Implement this function {}", __FUNCSIG__);
		}

		void DX12CommandRecorder::copy(Buffer* pSource, uint64_t srcOffset, Buffer* pDestination, uint64_t dstOffset, uint64_t size)
		{
			m_pCurrentCommandList->CopyBufferRegion(pDestination->as<DX12Buffer>()->getResource(), dstOffset, pSource->as<DX12Buffer>()->getResource(), srcOffset, size);
		}

		void DX12CommandRecorder::copy(Image* pSource, Swapchain* pDestination)
		{
			XENON_TODO_NOW("(Dhiraj) Implement this function {}", __FUNCSIG__);
			// m_pCurrentCommandList->CopyResource(pDestination->as<DX12Swapchain>()->getCurrentSwapchainImageResource(), pSource->as<DX12Image>()->getResource());
		}

		void DX12CommandRecorder::copy(Buffer* pSource, uint64_t bufferOffset, Image* pImage, glm::vec3 imageSize, glm::vec3 imageOffset /*= glm::vec3(0)*/)
		{
			XENON_TODO_NOW("(Dhiraj) Implement this function {}", __FUNCSIG__);
		}

		void DX12CommandRecorder::bind(Rasterizer* pRasterizer, const std::vector<Rasterizer::ClearValueType>& clearValues, bool usingSecondaryCommandRecorders /*= false*/)
		{
			const auto dxRasterizer = pRasterizer->as<DX12Rasterizer>();
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(dxRasterizer->getRenderTargetHeap()->GetCPUDescriptorHandleForHeapStart(), dxRasterizer->getFrameIndex(), dxRasterizer->getRenderTargetDescriptorSize());

			m_pCurrentCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
			// m_pCurrentCommandList->ClearRenderTargetView(rtvHandle, glm::value_ptr(std::get<glm::vec4>(clearValues.front())), 0, nullptr);

			m_IsRenderTargetBound = true;
		}

		void DX12CommandRecorder::bind(RasterizingPipeline* pPipeline, const VertexSpecification& vertexSpecification)
		{
			m_pCurrentCommandList->SetPipelineState(pPipeline->as<DX12RasterizingPipeline>()->getPipeline(vertexSpecification).m_PipelineState.Get());
		}

		void DX12CommandRecorder::bind(RasterizingPipeline* pPipeline, Descriptor* pUserDefinedDescriptor, Descriptor* pMaterialDescriptor, Descriptor* pCameraDescriptor)
		{
			XENON_TODO_NOW("(Dhiraj) Implement this function {}", __FUNCSIG__);
			// m_pCurrentCommandList->SetGraphicsRootDescriptorTable(0, pUserDefinedDescriptor->as<DX12Descriptor>());
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
			XENON_TODO_NOW("(Dhiraj) Implement this function {}", __FUNCSIG__);
		}

		void DX12CommandRecorder::executeChildren()
		{
			XENON_TODO_NOW("(Dhiraj) Implement this function {}", __FUNCSIG__);
		}

		void DX12CommandRecorder::end()
		{
			XENON_DX12_ASSERT(m_pCurrentCommandList->Close(), "Failed to stop the current command list!");
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