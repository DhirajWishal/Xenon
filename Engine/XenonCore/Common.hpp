// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <typeindex>
#include <string_view>

#define XENON_BIT_SHIFT(x)							(1 << x)

#define XENON_DISABLE_COPY(object)															\
	object(const object&) = delete;															\
	object& operator=(const object&) = delete

#define XENON_DISABLE_MOVE(object)															\
	object(object&&) = delete;																\
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
	[[nodiscard]] constexpr std::type_index GetTypeIndex() { return std::type_index(typeid(Type)); }

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
	[[nodiscard]] constexpr bool EnumContains(Enum value, Enums... enums)
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
	[[nodiscard]] constexpr std::underlying_type_t<Type> EnumToInt(Type value) { return static_cast<std::underlying_type_t<Type>>(value); }

	/**
	 * Generate hash for a set of bytes.
	 *
	 * @param pBytes The bytes to hash.
	 * @param size The number of bytes to hash.
	 * @param seed The hash seed. Default is 0.
	 * @return The 64-bit hash value.
	 */
	[[nodiscard]] uint64_t GenerateHash(const std::byte* pBytes, uint64_t size, uint64_t seed = 0);
}

#define XENON_DEFINE_ENUM_AND(name)															\
	constexpr bool operator&(const name lhs, const name rhs)								\
	{																						\
		return ::Xenon::EnumToInt(lhs) & ::Xenon::EnumToInt(rhs);							\
	}

#define XENON_DEFINE_ENUM_OR(name)															\
	constexpr name operator|(const name lhs, const name rhs)								\
	{																						\
		return static_cast<name>(::Xenon::EnumToInt(lhs) | ::Xenon::EnumToInt(rhs));		\
	}