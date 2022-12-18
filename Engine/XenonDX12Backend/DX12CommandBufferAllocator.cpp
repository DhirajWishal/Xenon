// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12CommandBufferAllocator.hpp"
#include "DX12Macros.hpp"
#include "DX12CommandBuffer.hpp"

namespace Xenon
{
	namespace Backend
	{
		DX12CommandBufferAllocator::DX12CommandBufferAllocator(DX12Device* pDevice, CommandBufferAllocatorUsage usage, uint8_t bufferCount)
			: CommandBufferAllocator(pDevice, usage, bufferCount)
			, DX12DeviceBoundObject(pDevice)
		{
			D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			if (usage & CommandBufferAllocatorUsage::Secondary)
				type = D3D12_COMMAND_LIST_TYPE_BUNDLE;

			else if (usage & CommandBufferAllocatorUsage::Compute)
				type = D3D12_COMMAND_LIST_TYPE_COMPUTE;

			else if (usage & CommandBufferAllocatorUsage::Transfer)
				type = D3D12_COMMAND_LIST_TYPE_COPY;

			// Create the command allocator.
			XENON_DX12_ASSERT(m_pDevice->getDevice()->CreateCommandAllocator(type, IID_PPV_ARGS(&m_Allocator)), "Failed to create the command allocator!");

			// Setup the command buffers.
			m_CommandBuffers.reserve(bufferCount);
			for (uint8_t i = 0; i < bufferCount; i++)
				m_CommandBuffers.emplace_back(m_pDevice, this);
		}

		DX12CommandBufferAllocator::~DX12CommandBufferAllocator()
		{
			m_CommandBuffers.clear();
		}

		CommandBuffer* DX12CommandBufferAllocator::getBuffer(uint8_t index)
		{
			return &m_CommandBuffers[index];
		}

		const CommandBuffer* DX12CommandBufferAllocator::getBuffer(uint8_t index) const
		{
			return &m_CommandBuffers[index];
		}
	}
}