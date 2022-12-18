// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/CommandBufferAllocator.hpp"

#include "DX12DeviceBoundObject.hpp"

namespace Xenon
{
	namespace Backend
	{
		class DX12CommandBuffer;

		/**
		 * DirectX 12 command buffer allocator class.
		 */
		class DX12CommandBufferAllocator final : public CommandBufferAllocator, public DX12DeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevcie The device pointer.
			 * @param usage The allocator usage.
			 * @param bufferCount The command buffer count.
			 */
			explicit DX12CommandBufferAllocator(DX12Device* pDevice, CommandBufferAllocatorUsage usage, uint8_t bufferCount);

			/**
			 * Destructor.
			 */
			~DX12CommandBufferAllocator() override;

			/**
			 * Get the command buffer pointer.
			 *
			 * @param index The index to access.
			 * @return The command buffer pointer.
			 */
			[[nodiscard]] CommandBuffer* getBuffer(uint8_t index) override;

			/**
			 * Get the command buffer pointer.
			 *
			 * @param index The index to access.
			 * @return The command buffer pointer.
			 */
			[[nodiscard]] const CommandBuffer* getBuffer(uint8_t index) const override;

			/**
			 * Get the command allocator.
			 *
			 * @return The allocator pointer.
			 */
			[[nodiscard]] ID3D12CommandAllocator* getCommandAllocator() noexcept { return m_Allocator.Get(); }

			/**
			 * Get the command allocator.
			 *
			 * @return The allocator pointer.
			 */
			[[nodiscard]] const ID3D12CommandAllocator* getCommandAllocator() const noexcept { return m_Allocator.Get(); }

		private:
			ComPtr<ID3D12CommandAllocator> m_Allocator;

			std::vector<DX12CommandBuffer> m_CommandBuffers;
		};
	}
}