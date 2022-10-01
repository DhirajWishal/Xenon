// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Buffer type enum.
		 */
		enum class BufferType : uint8_t
		{
			Index,
			Vertex,
			Staging,
			Storage,
			Uniform,
		};

		/**
		 * Buffer class.
		 * This is the base class for all the backend buffers used by Xenon.
		 */
		class Buffer : public BackendObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param size The size of the buffer in bytes.
			 * @param type The buffer type.
			 */
			explicit Buffer([[maybe_unused]] Device* pDevice, uint64_t size, BufferType type) : m_Size(size), m_Type(type) {}

			/**
			 * Default virtual destructor.
			 */
			virtual ~Buffer() = default;

			/**
			 * Copy data from another buffer to this buffer.
			 *
			 * @param pBuffer The buffer to copy the data from.
			 * @param size The size in bytes to copy.
			 * @param srcOffset The source buffer's offset. Default is 0.
			 * @param dstOffset The destination buffer's (this) offset. Default is 0.
			 */
			virtual void copy(const Buffer* pBuffer, uint64_t size, uint64_t srcOffset = 0, uint64_t dstOffset = 0) = 0;

		public:
			/**
			 * Get the byte size of the buffer.
			 *
			 * @return The size.
			 */
			[[nodiscard]] uint64_t getSize() const { return m_Size; }

			/**
			 * Get the buffer type.
			 *
			 * @return The buffer type.
			 */
			[[nodiscard]] BufferType getType() const { return m_Type; }

		protected:
			uint64_t m_Size = 0;
			BufferType m_Type;
		};
	}
}