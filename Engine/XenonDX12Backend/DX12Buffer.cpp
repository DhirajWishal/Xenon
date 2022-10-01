// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12Buffer.hpp"
#include "DX12Macros.hpp"

#include "DX12VertexBuffer.hpp"
#include "DX12IndexBuffer.hpp"
#include "DX12StagingBuffer.hpp"
#include "DX12StorageBuffer.hpp"
#include "DX12UniformBuffer.hpp"

namespace Xenon
{
	namespace Backend
	{
		DX12Buffer::DX12Buffer(DX12Device* pDevice, uint64_t size, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES resourceStates, D3D12_RESOURCE_FLAGS resourceFlags /*= D3D12_RESOURCE_FLAG_NONE*/)
			: m_pDevice(pDevice)
		{
			const auto resourceDescriptor = CD3DX12_RESOURCE_DESC::Buffer(size, resourceFlags);

			D3D12MA::ALLOCATION_DESC allocationDesc = {};
			allocationDesc.HeapType = heapType;

			XENON_DX12_ASSERT(pDevice->getAllocator()->CreateResource(
				&allocationDesc,
				&resourceDescriptor,
				D3D12_RESOURCE_STATE_COPY_SOURCE | D3D12_RESOURCE_STATE_COPY_DEST | resourceStates,
				nullptr,
				&m_pAllocation,
				IID_NULL,
				nullptr), "Failed to create the DirectX 12 buffer!");
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
			commandList->CopyBufferRegion(m_Buffer.Get(), dstOffset, pBuffer->m_Buffer.Get(), srcOffset, size);

			// End the command list.
			XENON_DX12_ASSERT(commandList->Close(), "Failed to stop the current DirectX 12 command list!");

			// Finally, submit the command list to be executed.
			ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
			m_pDevice->getCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		}
	}
}