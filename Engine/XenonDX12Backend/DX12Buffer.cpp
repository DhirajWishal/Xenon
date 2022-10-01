// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12Buffer.hpp"
#include "DX12Macros.hpp"

#include "DX12VertexBuffer.hpp"
#include "DX12IndexBuffer.hpp"
#include "DX12StagingBuffer.hpp"
#include "DX12StorageBuffer.hpp"
#include "DX12UniformBuffer.hpp"

#ifdef max
#undef max

#endif

namespace Xenon
{
	namespace Backend
	{
		DX12Buffer::DX12Buffer(DX12Device* pDevice, uint64_t size, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES resourceStates, D3D12_RESOURCE_FLAGS resourceFlags /*= D3D12_RESOURCE_FLAG_NONE*/)
			: m_pDevice(pDevice)
			, m_Size(size)
		{
			const auto resourceDescriptor = CD3DX12_RESOURCE_DESC::Buffer(size, resourceFlags);

			D3D12MA::ALLOCATION_DESC allocationDesc = {};
			allocationDesc.HeapType = heapType;

			XENON_DX12_ASSERT(pDevice->getAllocator()->CreateResource(
				&allocationDesc,
				&resourceDescriptor,
				/*D3D12_RESOURCE_STATE_COPY_SOURCE | D3D12_RESOURCE_STATE_COPY_DEST | */resourceStates,
				nullptr,
				&m_pAllocation,
				IID_NULL,
				nullptr), "Failed to create the DirectX 12 buffer!");
		}

		DX12Buffer::~DX12Buffer()
		{
			m_pAllocation->Release();
		}

		DX12Buffer* DX12Buffer::From(Buffer* pBuffer)
		{
			switch (pBuffer->getType())
			{
			case Xenon::Backend::BufferType::Index:
				return pBuffer->as<DX12IndexBuffer>();

			case Xenon::Backend::BufferType::Vertex:
				return pBuffer->as<DX12VertexBuffer>();

			case Xenon::Backend::BufferType::Staging:
				return pBuffer->as<DX12StagingBuffer>();

			case Xenon::Backend::BufferType::Storage:
				return pBuffer->as<DX12StorageBuffer>();

			case Xenon::Backend::BufferType::Uniform:
				return pBuffer->as<DX12UniformBuffer>();

			default:
				XENON_LOG_ERROR("Invalid buffer type!");
				return nullptr;
			}
		}

		const DX12Buffer* DX12Buffer::From(const Buffer* pBuffer)
		{
			switch (pBuffer->getType())
			{
			case Xenon::Backend::BufferType::Index:
				return pBuffer->as<DX12IndexBuffer>();

			case Xenon::Backend::BufferType::Vertex:
				return pBuffer->as<DX12VertexBuffer>();

			case Xenon::Backend::BufferType::Staging:
				return pBuffer->as<DX12StagingBuffer>();

			case Xenon::Backend::BufferType::Storage:
				return pBuffer->as<DX12StorageBuffer>();

			case Xenon::Backend::BufferType::Uniform:
				return pBuffer->as<DX12UniformBuffer>();

			default:
				XENON_LOG_ERROR("Invalid buffer type!");
				return nullptr;
			}
		}

		void DX12Buffer::copyFrom(const DX12Buffer* pBuffer, uint64_t size, uint64_t srcOffset, uint64_t dstOffset)
		{
			// Create the command list.
			ComPtr<ID3D12GraphicsCommandList> commandList;
			XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pDevice->getCommandAllocator(), nullptr, IID_PPV_ARGS(&commandList)), "Failed to create the DirectX 12 copy command list!");

			// Copy the buffer region.
			commandList->CopyBufferRegion(m_pAllocation->GetResource(), dstOffset, pBuffer->m_pAllocation->GetResource(), srcOffset, size);

			// End the command list.
			XENON_DX12_ASSERT(commandList->Close(), "Failed to stop the current DirectX 12 command list!");

			// Submit the command list to be executed.
			ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
			m_pDevice->getCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

			// Wait till the command is done executing.
			ComPtr<ID3D12Fence> fence;
			XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)), "Failed to create the DirectX 12 fence!");
			XENON_DX12_ASSERT(m_pDevice->getCommandQueue()->Signal(fence.Get(), 1), "Failed to signal the DirectX 12 fence!");

			// Setup synchronization.
			auto fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

			// Validate the event.
			if (fenceEvent == nullptr)
			{
				XENON_LOG_ERROR("Failed to wait till the DirectX 12 command list execution!");
				return;
			}

			// Set the event and wait.
			XENON_DX12_ASSERT(fence->SetEventOnCompletion(1, fenceEvent), "Failed to set the DirectX 12 fence event on completion event!");
			WaitForSingleObjectEx(fenceEvent, std::numeric_limits<DWORD>::max(), FALSE);
			CloseHandle(fenceEvent);
		}

		void DX12Buffer::copyFrom(const std::byte* pData, uint64_t size, uint64_t offset /*= 0*/)
		{
			// First, create the copy buffer.
			auto copyBuffer = DX12Buffer(m_pDevice, size, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);

			// Map the memory.
			std::byte* copyBufferMemory = nullptr;
			copyBuffer.getResource()->Map(0, nullptr, reinterpret_cast<void**>(&copyBufferMemory));

			// Copy the data to the copy buffer.
			std::copy_n(pData, size, copyBufferMemory);

			// Unmap the copy buffer.
			copyBuffer.getResource()->Unmap(0, nullptr);

			// Finally copy everything to this.
			copyFrom(&copyBuffer, size, 0, offset);
		}

		const std::byte* DX12Buffer::map()
		{
			m_pTemporaryBuffer = std::make_unique<DX12Buffer>(m_pDevice, m_Size, D3D12_HEAP_TYPE_READBACK, D3D12_RESOURCE_STATE_COPY_DEST);
			m_pTemporaryBuffer->copyFrom(this, m_Size, 0, 0);

			std::byte* tempBufferMemory = nullptr;
			m_pTemporaryBuffer->getResource()->Map(0, nullptr, reinterpret_cast<void**>(&tempBufferMemory));
			return tempBufferMemory;
		}

		void DX12Buffer::unmap()
		{
			m_pTemporaryBuffer->getResource()->Unmap(0, nullptr);
		}
	}
}