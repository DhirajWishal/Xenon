// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.hpp"

namespace Xenon
{
	namespace Backend
	{
		class CommandBuffer;

		/**
		 * Command buffer allocator usage enum.
		 */
		enum class CommandBufferAllocatorUsage : uint8_t
		{
			Compute = XENON_BIT_SHIFT(0),
			Graphics = XENON_BIT_SHIFT(1),
			Transfer = XENON_BIT_SHIFT(2),
			Secondary = XENON_BIT_SHIFT(3)
		};

		XENON_DEFINE_ENUM_OR(CommandBufferAllocatorUsage);
		XENON_DEFINE_ENUM_AND(CommandBufferAllocatorUsage);

		/**
		 * Command buffer allocator class.
		 * This class is used to allocate command buffers which are used for different purposes.
		 */
		class CommandBufferAllocator : public BackendObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevcie The device pointer.
			 * @param usage The allocator usage.
			 * @param bufferCount The command buffer count.
			 */
			explicit CommandBufferAllocator([[maybe_unused]] const Device* pDevice, CommandBufferAllocatorUsage usage, uint8_t bufferCount) : m_Usage(usage), m_CommandBufferCount(bufferCount) {}

			/**
			 * Get the allocator usage.
			 *
			 * @return The command buffer allocator usage.
			 */
			[[nodiscard]] CommandBufferAllocatorUsage getUsage() const noexcept { return m_Usage; }

			/**
			 * Get the command buffer pointer.
			 *
			 * @param index The index to access.
			 * @return The command buffer pointer.
			 */
			[[nodiscard]] virtual CommandBuffer* getBuffer(uint8_t index) = 0;

			/**
			 * Get the command buffer pointer.
			 *
			 * @param index The index to access.
			 * @return The command buffer pointer.
			 */
			[[nodiscard]] virtual const CommandBuffer* getBuffer(uint8_t index) const = 0;

		protected:
			CommandBufferAllocatorUsage m_Usage = CommandBufferAllocatorUsage::Graphics;
			const uint8_t m_CommandBufferCount = 0;
		};
	}
}