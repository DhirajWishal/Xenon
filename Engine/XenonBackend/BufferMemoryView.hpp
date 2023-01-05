// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <iterator>

namespace Xenon
{
	namespace Backend
	{
		class Buffer;

		/**
		 * Buffer memory view class.
		 * This class can be used to access bytes of a buffer.
		 */
		class BufferMemoryView final
		{
		public:
			using iterator = const std::byte*;
			using const_iterator = iterator;
			using reverse_iterator = std::reverse_iterator<const std::byte*>;
			using const_reverse_iterator = std::reverse_iterator<const std::byte*>;

		public:
			/**
			 * Default constructor.
			 */
			constexpr BufferMemoryView() = default;

			/**
			 * Explicit constructor.
			 *
			 * @param pBuffer The host accessible buffer pointer.
			 */
			explicit BufferMemoryView(Buffer* pBuffer);

			/**
			 * Move constructor.
			 *
			 * @param other The other memory view object.
			 */
			BufferMemoryView(BufferMemoryView&& other) noexcept;

			/**
			 * Destructor.
			 */
			~BufferMemoryView();

			/**
			 * Get the source buffer pointer.
			 *
			 * @return The buffer pointer.
			 */
			[[nodiscard]] Buffer* getBuffer() { return m_pSourceBuffer; }

			/**
			 * Get the source buffer pointer.
			 *
			 * @return The const buffer pointer.
			 */
			[[nodiscard]] const Buffer* getBuffer() const { return m_pSourceBuffer; }

			/**
			 * Get a byte value at a given index.
			 *
			 * @param index The index position to access.
			 * @return The byte value of the position.
			 */
			[[nodiscard]] std::byte at(uint64_t index) const;

		public:
			/**
			 * Begin function to get the begin iterator.
			 *
			 * @return The begin pointer.
			 */
			[[nodiscard]] const std::byte* begin() const { return m_pBegin; }

			/**
			 * End function to get the end iterator.
			 *
			 * @return The end pointer.
			 */
			[[nodiscard]] const std::byte* end() const { return m_pEnd; }

			/**
			 * Const-begin function to get the const-begin iterator.
			 *
			 * @return The const begin pointer.
			 */
			[[nodiscard]] const std::byte* cbegin() const { return m_pBegin; }

			/**
			 * Const-end function to get the const-end iterator.
			 *
			 * @return The const end pointer.
			 */
			[[nodiscard]] const std::byte* cend() const { return m_pEnd; }

			/**
			 * Reverse begin function to get the reverse begin iterator.
			 *
			 * @return The reverse begin iterator.
			 */
			[[nodiscard]] const reverse_iterator rbegin() const { return reverse_iterator(m_pBegin); }

			/**
			 * Reverse end function to get the reverse end iterator.
			 *
			 * @return The reverse end iterator.
			 */
			[[nodiscard]] const reverse_iterator rend() const { return reverse_iterator(m_pEnd); }

			/**
			 * Reverse const-begin function to get the reverse const-begin iterator.
			 *
			 * @return The reverse const begin iterator.
			 */
			[[nodiscard]] const const_reverse_iterator crbegin() const { return const_reverse_iterator(m_pBegin); }

			/**
			 * Reverse const-end function to get the reverse const-end iterator.
			 *
			 * @return The reverse const end iterator.
			 */
			[[nodiscard]] const const_reverse_iterator crend() const { return const_reverse_iterator(m_pEnd); }

		public:
			/**
			 * Subscript operator.
			 *
			 * @param index The index position to access.
			 * @return The byte value of the position.
			 */
			[[nodiscard]] std::byte operator[](uint64_t index) const;

			/**
			 * Move assignment operator.
			 *
			 * @param other The other memory view object.
			 * @return The assigned memory view object.
			 */
			BufferMemoryView& operator=(BufferMemoryView&& other) noexcept;

		private:
			Buffer* m_pSourceBuffer = nullptr;

			std::byte const* m_pBegin = nullptr;
			std::byte const* m_pEnd = nullptr;
		};
	}
}