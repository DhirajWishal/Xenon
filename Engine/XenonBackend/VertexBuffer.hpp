// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Buffer.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vertex buffer class.
		 * This class is used to store vertex information.
		 */
		class VertexBuffer : public Buffer
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param size The size of the buffer in bytes.
			 * @param stride The byte size of a single vertex entry.
			 */
			explicit VertexBuffer(Device* pDevice, uint64_t size, uint64_t stride) : Buffer(pDevice, size, BufferType::Vertex), m_Stride(stride) {}

			/**
			 * Default virtual destructor.
			 */
			virtual ~VertexBuffer() = default;

		public:
			/**
			 * Get the number of vertices stored in the buffer.
			 *
			 * @return The vertex count.
			 */
			[[nodiscard]] uint64_t getVertexCount() const { return m_Size / m_Stride; }

		protected:
			uint64_t m_Stride = 0;
		};
	}
}