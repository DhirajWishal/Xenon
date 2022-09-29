// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Buffer.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Index size enum.
		 * This contains all the available sizes of a single index entry.
		 */
		enum class IndexSize : uint8_t
		{
			Undefined,
			Uint8,
			Uint16,
			Uint32,
		};

		/**
		 * Index buffer class.
		 * This class is used to store index information.
		 */
		class IndexBuffer : public Buffer
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param size The size of the buffer in bytes.
			 * @param indexSize The size of a single index.
			 */
			explicit IndexBuffer(Device* pDevice, uint64_t size, IndexSize indexSize) : Buffer(pDevice, size, BufferType::Index), m_IndexSize(indexSize) {}

			/**
			 * Default virtual destructor.
			 */
			virtual ~IndexBuffer() = default;

		public:
			/**
			 * Get the number of indices stored in the buffer.
			 *
			 * @return The index count.
			 */
			[[nodiscard]] uint64_t getIndexCount() const { return m_Size / static_cast<uint8_t>(m_IndexSize); }

		protected:
			IndexSize m_IndexSize = IndexSize::Undefined;
		};
	}
}