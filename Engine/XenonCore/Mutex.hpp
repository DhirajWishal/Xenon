// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mutex>

namespace Xenon
{
	/**
	 * Mutex class.
	 *
	 * @tparam Type The data type to synchronize.
	 * @tparam MutexType The mutex type to use. Default is std::mutex.
	 */
	template<class Type, class MutexType = std::mutex>
	class Mutex final
	{
	public:
		/**
		 * Default constructor.
		 */
		Mutex() = default;

		/**
		 * Explicit constructor.
		 *
		 * @param data The data to initialize.
		 */
		Mutex(const Type& data) : m_Data(data) {}

		/**
		 * Explicit constructor.
		 *
		 * @param data The data to initialize.
		 */
		Mutex(Type&& data) : m_Data(std::move(data)) {}

		/**
		 * Copy constructor.
		 *
		 * @param other The other mutex.
		 */
		Mutex(const Mutex& other)
		{
			auto lock = std::scoped_lock(m_Mutex);
			m_Data = other.m_Data;
		}

		/**
		 * Move constructor.
		 *
		 * @param other The other mutex.
		 */
		Mutex(Mutex&& other) noexcept
		{
			auto lock = std::scoped_lock(m_Mutex);
			m_Data = std::move(other.m_Data);
		}

		/**
		 * Set the data.
		 *
		 * @param data The data to set.
		 */
		void set(const Type& data)
		{
			auto lock = std::scoped_lock(m_Mutex);
			m_Data = data;
		}

		/**
		 * Set the data.
		 *
		 * @param data The data to set.
		 */
		void set(Type&& data)
		{
			auto lock = std::scoped_lock(m_Mutex);
			m_Data = std::move(data);
		}

		/**
		 * Safely access the internally stored data.
		 * The function can have a return and optional arguments can be passed to it. But make sure that the first argument is always the required variable to access.
		 *
		 * @tparam Function The function type.
		 * @tparam Arguments The argument types.
		 * @param function The function which can safely access the variable.
		 * @param arguments The arguments to forward to the function.
		 * @return The function's return.
		 */
		template<class Function, class... Arguments>
		decltype(auto) access(Function&& function, Arguments&&... arguments)
		{
			auto lock = std::scoped_lock(m_Mutex);
			return function(m_Data, std::forward<Arguments>(arguments)...);
		}

		/**
		 * Get a copy from the internally stored data safely.
		 *
		 * @retrun The data copy.
		 */
		[[nodiscard]] Type get() const
		{
			auto lock = std::scoped_lock(m_Mutex);
			return m_Data;
		}

		/**
		 * Get the data reference.
		 * Note that this operation is unsafe.
		 *
		 * @return The data reference.
		 */
		[[nodiscard]] Type& getUnsafe() { return m_Data; }

		/**
		 * Get the data reference.
		 * Note that this operation is unsafe.
		 *
		 * @return The data reference.
		 */
		[[nodiscard]] const Type& getUnsafe() const { return m_Data; }

	public:
		/**
		 * Assignment operator overload.
		 *
		 * @param data The data to assign.
		 * @return The assigned data.
		 */
		Mutex& operator=(const Mutex& data)
		{
			m_Mutex = data.m_Mutex;
			auto lock = std::scoped_lock(m_Mutex);
			m_Data = data.m_Data;
			return *this;
		}

		/**
		 * Assignment operator overload.
		 *
		 * @param data The data to assign.
		 * @return The assigned data.
		 */
		Mutex& operator=(Mutex&& data) noexcept
		{
			m_Mutex = std::move(data.m_Mutex);
			auto lock = std::scoped_lock(m_Mutex);
			m_Data = std::move(data.m_Data);
			return *this;
		}

		/**
		 * Assignment operator overload.
		 *
		 * @param data The data to assign.
		 * @return The assigned data.
		 */
		Type& operator=(const Type& data)
		{
			auto lock = std::scoped_lock(m_Mutex);
			m_Data = data;
			return m_Data;
		}

		/**
		 * Assignment operator overload.
		 *
		 * @param data The data to assign.
		 * @return The assigned data.
		 */
		Type& operator=(Type&& data)
		{
			auto lock = std::scoped_lock(m_Mutex);
			m_Data = std::move(data);
			return m_Data;
		}

	private:
		Type m_Data;
		MutexType m_Mutex;
	};
}