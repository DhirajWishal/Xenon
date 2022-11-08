// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12Buffer.hpp"
#include "DX12Macros.hpp"

#ifdef max
#undef max

#endif

namespace Xenon
{
	namespace Backend
	{
		DX12Buffer::DX12Buffer(DX12Device* pDevice, uint64_t size, BufferType type)
			:DX12DeviceBoundObject(pDevice)
			, Buffer(pDevice, size, type)
			, m_Size(size)
		{
			D3D12MA::ALLOCATION_DESC allocationDesc = {};
			CD3DX12_RESOURCE_DESC resourceDescriptor = {};
			D3D12_RESOURCE_STATES resourceStates = {};

			switch (type)
			{
			case Xenon::Backend::BufferType::Index:
				resourceStates = D3D12_RESOURCE_STATE_INDEX_BUFFER;
				allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
				resourceDescriptor = CD3DX12_RESOURCE_DESC::Buffer(size, D3D12_RESOURCE_FLAG_NONE);
				break;

			case Xenon::Backend::BufferType::Vertex:
				resourceStates = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
				allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
				resourceDescriptor = CD3DX12_RESOURCE_DESC::Buffer(size, D3D12_RESOURCE_FLAG_NONE);
				break;

			case Xenon::Backend::BufferType::Staging:
				resourceStates = D3D12_RESOURCE_STATE_COMMON;
				allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
				resourceDescriptor = CD3DX12_RESOURCE_DESC::Buffer(size, D3D12_RESOURCE_FLAG_NONE);
				break;

			case Xenon::Backend::BufferType::Storage:
				resourceStates = D3D12_RESOURCE_STATE_COMMON;
				allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
				resourceDescriptor = CD3DX12_RESOURCE_DESC::Buffer(size, D3D12_RESOURCE_FLAG_NONE);
				break;

			case Xenon::Backend::BufferType::Uniform:
				resourceStates = D3D12_RESOURCE_STATE_COMMON;
				allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
				resourceDescriptor = CD3DX12_RESOURCE_DESC::Buffer(size, D3D12_RESOURCE_FLAG_NONE);
				break;

			default:
				m_Type = BufferType::Staging;
				resourceStates = D3D12_RESOURCE_STATE_COMMON;
				allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
				resourceDescriptor = CD3DX12_RESOURCE_DESC::Buffer(size, D3D12_RESOURCE_FLAG_NONE);
				XENON_LOG_ERROR("Invalid or unsupported buffer type! Defaulting to staging.");
				break;
			}

			XENON_DX12_ASSERT(pDevice->getAllocator()->CreateResource(
				&allocationDesc,
				&resourceDescriptor,
				/*D3D12_RESOURCE_STATE_COPY_SOURCE | D3D12_RESOURCE_STATE_COPY_DEST | */resourceStates,
				nullptr,
				&m_pAllocation,
				IID_NULL,
				nullptr), "Failed to create the buffer!");
		}

		DX12Buffer::DX12Buffer(DX12Device* pDevice, uint64_t size, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES resourceStates, D3D12_RESOURCE_FLAGS resourceFlags /*= D3D12_RESOURCE_FLAG_NONE*/)
			:DX12DeviceBoundObject(pDevice)
			, Buffer(pDevice, size, BufferType::BackendSpecific)
			, m_Size(size)
		{
			CD3DX12_RESOURCE_DESC resourceDescriptor = CD3DX12_RESOURCE_DESC::Buffer(size, resourceFlags);

			D3D12MA::ALLOCATION_DESC allocationDesc = {};
			allocationDesc.HeapType = heapType;

			XENON_DX12_ASSERT(pDevice->getAllocator()->CreateResource(
				&allocationDesc,
				&resourceDescriptor,
				/*D3D12_RESOURCE_STATE_COPY_SOURCE | D3D12_RESOURCE_STATE_COPY_DEST | */resourceStates,
				nullptr,
				&m_pAllocation,
				IID_NULL,
				nullptr), "Failed to create the buffer!");
		}

		DX12Buffer::~DX12Buffer()
		{
			try
			{
				m_pDevice->getInstance()->getDeletionQueue().insert([allocation = m_pAllocation] { allocation->Release(); });
			}
			catch (...)
			{
				XENON_DX12_ASSERT(-1, "Failed to push the buffer deletion function to the deletion queue!");
			}
		}

		void DX12Buffer::copy(Buffer* pBuffer, uint64_t size, uint64_t srcOffset /*= 0*/, uint64_t dstOffset /*= 0*/)
		{
			// Create the command list.
			ComPtr<ID3D12GraphicsCommandList> commandList;
			XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pDevice->getCommandAllocator(), nullptr, IID_PPV_ARGS(&commandList)), "Failed to create the copy command list!");

			// Copy the buffer region.
			commandList->CopyBufferRegion(m_pAllocation->GetResource(), dstOffset, pBuffer->as<DX12Buffer>()->m_pAllocation->GetResource(), srcOffset, size);

			// End the command list.
			XENON_DX12_ASSERT(commandList->Close(), "Failed to stop the current command list!");

			// Submit the command list to be executed.
			ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
			m_pDevice->getCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

			// Wait till the command is done executing.
			ComPtr<ID3D12Fence> fence;
			XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)), "Failed to create the fence!");
			XENON_DX12_ASSERT(m_pDevice->getCommandQueue()->Signal(fence.Get(), 1), "Failed to signal the fence!");

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

		void DX12Buffer::write(const std::byte* pData, uint64_t size, uint64_t offset /*= 0*/)
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
			copy(&copyBuffer, size, 0, offset);
		}

		const std::byte* DX12Buffer::beginRead()
		{
			return map();
		}

		void DX12Buffer::endRead()
		{
			unmap();
		}

		const std::byte* DX12Buffer::map()
		{
			m_pTemporaryBuffer = std::make_unique<DX12Buffer>(m_pDevice, m_Size, D3D12_HEAP_TYPE_READBACK, D3D12_RESOURCE_STATE_COPY_DEST);
			m_pTemporaryBuffer->copy(this, m_Size, 0, 0);

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