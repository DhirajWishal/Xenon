// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DX12CommandAllocator.hpp"
#include "DX12Macros.hpp"

namespace Xenon
{
	namespace Backend
	{
		DX12CommandAllocator::DX12CommandAllocator(DX12Device* pDevice)
		{
			// Create the allocator.
			XENON_DX12_ASSERT(pDevice->getDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator)), "Failed to create the DirectX 12 command allocator!");
		}
	}
}