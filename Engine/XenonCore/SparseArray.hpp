// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>
#include <cstdint>

namespace Xenon
{
	/**
	 * Sparse array class.
	 * Sparse arrays contain three internal vectors.
	 * 1. Dense array: This contains the actual data that's been stored.
	 * 2. Sparse array: This contains the index map.
	 * 3. Availability map: This contains the information about which sparse array indexes are valid and which are false.
	 *
	 * This class works by storing the actual data in the dense array, and storing the index of that entry in the sparse array.
	 * Later the index of the sparse array can be used to index the sparse array, which gives the index of the dense array
	 * where the required value is stored in. The availability map represents an efficient way of storing information about
	 * which sparse array entries are valid, and can be used to delete unwanted memory.
	 *
	 * @tparam Type The value type.
	 * @tparam IndexType The index type.
	 */
	template<class Type, class IndexType = uint64_t>
	class SparseArray final
	{
	public:
		using value_type = std::vector<Type>::value_type;
		using allocator_type = std::vector<Type>::allocator_type;
		using size_type = std::vector<Type>::size_type;
		using difference_type = std::vector<Type>::difference_type;
		using reference = std::vector<Type>::reference;
		using const_reference = std::vector<Type>::const_reference;
		using pointer = std::vector<Type>::pointer;
		using const_pointer = std::vector<Type>::const_pointer;
		using iterator = std::vector<Type>::iterator;
		using const_iterator = std::vector<Type>::const_iterator;
		using reverse_iterator = std::vector<Type>::reverse_iterator;
		using const_reverse_iterator = std::vector<Type>::const_reverse_iterator;

	public:
		/**
		 * Default constructor.
		 */
		SparseArray() = default;

		/**
		 * Get the element stored at a given index.
		 *
		 * @param index The index of the element.
		 * @return The value reference.
		 */
		XENON_NODISCARD Type& at(uint64_t index) { return m_DenseArray[m_SparseArray[index]]; }

		/**
		 * Get the element stored at a given index.
		 *
		 * @param index The index of the element.
		 * @return The const value reference.
		 */
		XENON_NODISCARD const Type& at(uint64_t index) const { return m_DenseArray[m_SparseArray[index]]; }

		/**
		 * Get the front element in the storage.
		 *
		 * @return The front value reference.
		 */
		XENON_NODISCARD Type& front() { return m_DenseArray.front(); }

		/**
		 * Get the front element in the storage.
		 *
		 * @return The front const value reference.
		 */
		XENON_NODISCARD const Type& front() const { return m_DenseArray.front(); }

		/**
		 * Get the back element in the storage.
		 *
		 * @return The back value reference.
		 */
		XENON_NODISCARD Type& back() { return m_DenseArray.back(); }

		/**
		 * Get the back element in the storage.
		 *
		 * @return The back const value reference.
		 */
		XENON_NODISCARD const Type& back() const { return m_DenseArray.back(); }

		/**
		 * Get the data pointer.
		 *
		 * @return The data pointer.
		 */
		XENON_NODISCARD Type* data() { return m_DenseArray.data(); }

		/**
		 * Get the data pointer.
		 *
		 * @return The const data pointer.
		 */
		XENON_NODISCARD const Type* data() const { return m_DenseArray.data(); }

	public:
		/**
		 * Get the begin pointer.
		 *
		 * @return The begin pointer.
		 */
		XENON_NODISCARD decltype(auto) begin() { return m_DenseArray.begin(); }

		/**
		 * Get the begin pointer.
		 *
		 * @return The begin pointer.
		 */
		XENON_NODISCARD decltype(auto) begin() const { return m_DenseArray.begin(); }

		/**
		 * Get the begin pointer.
		 *
		 * @return The begin pointer.
		 */
		XENON_NODISCARD decltype(auto) cbegin() { return m_DenseArray.cbegin(); }

		/**
		 * Get the end pointer.
		 *
		 * @return The end pointer.
		 */
		XENON_NODISCARD decltype(auto) end() { return m_DenseArray.end(); }

		/**
		 * Get the end pointer.
		 *
		 * @return The end pointer.
		 */
		XENON_NODISCARD decltype(auto) cend() const { return m_DenseArray.cend(); }

		/**
		 * Get the end pointer.
		 *
		 * @return The end pointer.
		 */
		XENON_NODISCARD decltype(auto) cend() { return m_DenseArray.cend(); }

		/**
		 * Get the reverse begin pointer.
		 *
		 * @return The reverse begin pointer.
		 */
		XENON_NODISCARD decltype(auto) rbegin() { return m_DenseArray.rbegin(); }

		/**
		 * Get the reverse begin pointer.
		 *
		 * @return The reverse begin pointer.
		 */
		XENON_NODISCARD decltype(auto) rbegin() const { return m_DenseArray.rbegin(); }

		/**
		 * Get the reverse begin pointer.
		 *
		 * @return The reverse begin pointer.
		 */
		XENON_NODISCARD decltype(auto) rcbegin() { return m_DenseArray.rcbegin(); }

		/**
		 * Get the reverse end pointer.
		 *
		 * @return The reverse end pointer.
		 */
		XENON_NODISCARD decltype(auto) rend() { return m_DenseArray.rend(); }

		/**
		 * Get the reverse end pointer.
		 *
		 * @return The reverse end pointer.
		 */
		XENON_NODISCARD decltype(auto) rcend() const { return m_DenseArray.rcend(); }

		/**
		 * Get the reverse end pointer.
		 *
		 * @return The reverse end pointer.
		 */
		XENON_NODISCARD decltype(auto) rcend() { return m_DenseArray.rcend(); }

	public:
		/**
		 * Check if the array is empty.
		 *
		 * @return True if the array is empty.
		 * @return False if the array is not empty.
		 */
		XENON_NODISCARD bool empty() const { return m_DenseArray.empty(); }

		/**
		 * Get the size of the array.
		 *
		 * @return The size of the array.
		 */
		XENON_NODISCARD uint64_t size() const { return m_DenseArray.size(); }

	public:
		/**
		 * Insert a new entry to the sparse array.
		 *
		 * @tparam Arguments The argument types.
		 * @param arguments The arguments.
		 * @return The index and the value pointer pair.
		 */
		template<class...Arguments>
		XENON_NODISCARD std::pair<IndexType, Type*> insert(Arguments&&... arguments)
		{
			const auto index = m_SparseArray.size();
			m_SparseArray.emplace_back(m_DenseArray.size());
			m_AvailabilityMap.emplace_back(true);

			auto& value = m_DenseArray.emplace_back(std::forward<Arguments>(arguments)...);
			return std::make_pair(index, &value);
		}

		/**
		 * Remove the element at the given index.
		 * It will also try and clear the sparse array if required. This operation might be a little costly.
		 *
		 * @param index The index to remove the element from.
		 * @param shouldClear Whether we should clean the array. Default is true.
		 */
		void remove(uint64_t index, bool shouldClear = true)
		{
			m_DenseArray.erase(m_DenseArray.begin() + m_SparseArray[index]);
			m_AvailabilityMap[index] = false;

			if (shouldClear)
				clean();
		}

	private:
		/**
		 * Clean the sparse array and availability map if the last entries are free.
		 */
		void clean()
		{
			const auto finalIndex = m_SparseArray.size() - 1;

			// Find the last free index in the availability map.
			uint64_t lastFreeIndex = finalIndex;
			for (uint64_t index = 0; index < m_SparseArray.size(); index++)
			{
				if (!m_AvailabilityMap[index])
					lastFreeIndex = index;
			}

			// If the last free index is less than the last element index, let's remove it.
			if (lastFreeIndex < finalIndex)
			{
				m_SparseArray.erase(m_SparseArray.begin() + lastFreeIndex, m_SparseArray.end());
				m_AvailabilityMap.erase(m_AvailabilityMap.begin() + lastFreeIndex, m_AvailabilityMap.end());
			}
		}

	public:
		/**
		 * Get the element stored at a given index.
		 *
		 * @param index The index of the element.
		 * @return The value reference.
		 */
		XENON_NODISCARD Type& operator[](uint64_t index) { return m_DenseArray[m_SparseArray[index]]; }

		/**
		 * Get the element stored at a given index.
		 *
		 * @param index The index of the element.
		 * @return The const value reference.
		 */
		XENON_NODISCARD const Type& operator[](uint64_t index) const { return m_DenseArray[m_SparseArray[index]]; }

	private:
		std::vector<Type> m_DenseArray;
		std::vector<IndexType> m_SparseArray;
		std::vector<bool> m_AvailabilityMap;
	};
}