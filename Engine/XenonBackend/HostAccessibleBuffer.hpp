// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Buffer.hpp"
#include "BufferMemoryView.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Host accessible buffer class.
		 * This buffer type can be used by the host (the user) to access data directly.
		 */
		class HostAccessibleBuffer : public Buffer
		{
			friend BufferMemoryView;

		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param size The size of the buffer in bytes.
			 * @param type The buffer type.
			 */
			explicit HostAccessibleBuffer(Device* pDevice, uint64_t size, BufferType type) : Buffer(pDevice, size, type) {}

			/**
			 * Default virtual destructor.
			 */
			virtual ~HostAccessibleBuffer() = default;

			/**
			 * Write data to the buffer.
			 *
			 * @param pData The data pointer to copy the data from.
			 * @param size The size of the data to copy in bytes.
			 * @param offset The buffer's offset to copy to. Default is 0.
			 */
			virtual void write(const std::byte* pData, uint64_t size, uint64_t offset = 0) = 0;

			/**
			 * Read data from the buffer.
			 *
			 * @return The buffer's memory view.
			 */
			[[nodiscard]] BufferMemoryView read() { return BufferMemoryView(this); }

		protected:
			/**
			 * Begin reading data from the GPU.
			 *
			 * @return The const data pointer.
			 */
			[[nodiscard]] virtual const std::byte* beginRead() = 0;

			/**
			 * End the buffer reading.
			 */
			virtual void endRead() = 0;
		};
	}
}