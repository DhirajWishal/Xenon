// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <typeindex>

/**
 * This macro will disable the object from copying.
 *
 * @param object The object type.
 */
#define XENON_DISABLE_COPY(object)				\
object(const object&) = delete;					\
object& operator=(const object&) = delete

 /**
  * This macro will disable the object from moving.
  *
  * @param object The object type.
  */
#define XENON_DISABLE_MOVE(object)				\
object(object&&) = delete;						\
object& operator=(object&&) = delete 

namespace Xenon
{
	/**
	 * Utility function to get the type index of a type.
	 *
	 * @tparam Type The type to get the type index of.
	 * @return The type index.
	 */
	template<class Type>
	constexpr std::type_index GetTypeIndex() { return std::type_index(typeid(Type)); }

	/**
	 * Check if an enum contains multiple values.
	 *
	 * @tparam Enum The enum type.
	 * @tparam Enums The enum types.
	 * @param value The value to check.
	 * @param enums The enum values to check.
	 * @return True if all the enums are in the value.
	 * @return False if all the enums are not in the value.
	 */
	template<class Enum, class...Enums>
	constexpr bool EnumContains(Enum value, Enums... enums)
	{
		bool contains = false;
		const auto function = [value, &contains](Enum e) { contains |= value & e; };
		(function(enums), ...);

		return contains;
	}

	/**
	 * Convert an enum to it's underlying integer type.
	 *
	 * @tparam Type The enum type.
	 * @param value The value to convert.
	 * @return The integer value.
	 */
	template<class Type>
	constexpr std::underlying_type_t<Type> EnumToInt(Type value) { return static_cast<std::underlying_type_t<Type>>(value); }
}

/**
 * This macro defines the bitwise-AND operation operator for the specified enum.
 *
 * @param name The name of the enum.
 */
#define XENON_DEFINE_ENUM_AND(name)													\
constexpr bool operator&(const name lhs, const name rhs)							\
{																					\
	return ::Xenon::EnumToInt(lhs) & ::Xenon::EnumToInt(rhs);						\
}

 /**
  * This macro defines the bitwise-OR operation operator for the specified enum.
  *
  * @param name The name of the enum.
  */
#define XENON_DEFINE_ENUM_OR(name)													\
constexpr name operator|(const name lhs, const name rhs)							\
{																					\
	return static_cast<name>(::Xenon::EnumToInt(lhs) | ::Xenon::EnumToInt(rhs));	\
}