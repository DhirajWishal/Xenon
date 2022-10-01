// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DX12Device.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * DirectX 12 command buffers.
		 */
		class DX12CommandBuffers final
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 */
			explicit DX12CommandBuffers(DX12Device* pDevice);

		private:
			ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
			ComPtr<ID3D12GraphicsCommandList> m_CommandList;
		};
	}
}