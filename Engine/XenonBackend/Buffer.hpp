// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.hpp"
#include "BufferMemoryView.hpp"

namespace Xenon
{
	namespace Backend
	{
		class CommandRecorder;

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

			// The scratch buffer is used by acceleration structures to pass in geometry data.
			Scratch,

			// This buffer type is not available for normal use, but is only defined in backend(s).
			BackendSpecific
		};

		/**
		 * Buffer class.
		 * Buffers are used to store data for different purposes, including vertex data, index data, uniform data and even for transient usage.
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
			explicit Buffer([[maybe_unused]] const Device* pDevice, uint64_t size, BufferType type) : m_Size(size), m_Type(type) {}

			/**
			 * Copy data from another buffer to this buffer.
			 *
			 * @param pBuffer The buffer to copy the data from.
			 * @param size The size in bytes to copy.
			 * @param srcOffset The source buffer's offset. Default is 0.
			 * @param dstOffset The destination buffer's (this) offset. Default is 0.
			 */
			virtual void copy(Buffer* pBuffer, uint64_t size, uint64_t srcOffset = 0, uint64_t dstOffset = 0) = 0;

			/**
			 * Write data to the buffer.
			 *
			 * @param pData The data pointer to copy the data from.
			 * @param size The size of the data to copy in bytes.
			 * @param offset The buffer's offset to copy to. Default is 0.
			 * @param pCommandRecorder The command recorder used for internal transfer. Default is nullptr.
			 */
			virtual void write(const std::byte* pData, uint64_t size, uint64_t offset = 0, CommandRecorder* pCommandRecorder = nullptr) = 0;

			/**
			 * Utility function to copy an object to the buffer.
			 * This might be useful especially when copying uniform data.
			 *
			 * @tparam Type The object type.
			 * @param data The data to copy.
			 * @param offset The buffer's offset to copy to. Default is 0.
			 * @param pCommandRecorder The command recorder used for internal transfer. Default is nullptr.
			 */
			template<class Type>
			void writeObject(const Type& data, uint64_t offset = 0, CommandRecorder* pCommandRecorder = nullptr)
			{
				write(ToBytes(&data), sizeof(Type), offset, pCommandRecorder);
			}

			/**
			 * Read data from the buffer.
			 *
			 * @return The buffer's memory view.
			 */
			[[nodiscard]] BufferMemoryView read() { return BufferMemoryView(this); }

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