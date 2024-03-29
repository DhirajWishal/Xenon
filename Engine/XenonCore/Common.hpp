// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Features.hpp"

#include <typeindex>
#include <string_view>
#include <bit>

#define XENON_BIT_SHIFT(x)								(1 << x)
#define XENON_ALIGNED_SIZE_2(size, alignment)			(((size) + (alignment)-1) & ~((alignment)-1))

#define XENON_DISABLE_COPY(object)															\
	object(const object&) = delete;															\
	object& operator=(const object&) = delete

#define XENON_DISABLE_MOVE(object)															\
	object(object&&) = delete;																\
	object& operator=(object&&) = delete 

#ifdef XENON_FEATURE_RANGES
#	define XENON_NAMESPACE_RANGES	std::ranges
#	define XENON_RANGES(function, container, ...)		XENON_NAMESPACE_RANGES::function(container, __VA_ARGS__)

#else
#	define XENON_NAMESPACE_RANGES	std
#	define XENON_RANGES(function, container, ...)		XENON_NAMESPACE_RANGES::function(container.begin(), container.end(), __VA_ARGS__)

#endif

#ifdef XENON_FEATURE_BIT_CAST
#	define XENON_BIT_CAST(to, from)						std::bit_cast<to>(from)

#else
#	define XENON_BIT_CAST(to, from)						reinterpret_cast<to>(from)

#endif

#define XENON_NODISCARD									[[nodiscard]]
#define XENON_MAYBE_UNUSED								[[maybe_unused]]

namespace Xenon
{
	/**
	 * Utility function to get the type index of a type.
	 *
	 * @tparam Type The type to get the type index of.
	 * @return The type index.
	 */
	template<class Type>
	XENON_NODISCARD std::type_index GetTypeIndex() noexcept { return std::type_index(typeid(Type)); }

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
	XENON_NODISCARD constexpr bool EnumContains(Enum value, Enums... enums) noexcept
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
	XENON_NODISCARD constexpr std::underlying_type_t<Type> EnumToInt(Type value) noexcept { return static_cast<std::underlying_type_t<Type>>(value); }

	/**
	 * Cast a pointer to std::byte pointer.
	 *
	 * @tparam Type The pointer type.
	 * @param pointer The pointer to cast from.
	 * @return The std::byte pointer.
	 */
	template<class Type>
	XENON_NODISCARD constexpr std::byte* ToBytes(Type* pointer) noexcept { return XENON_BIT_CAST(std::byte*, pointer); }

	/**
	 * Cast a pointer to std::byte pointer.
	 *
	 * @tparam Type The pointer type.
	 * @param pointer The pointer to cast from.
	 * @return The std::byte pointer.
	 */
	template<class Type>
	XENON_NODISCARD constexpr const std::byte* ToBytes(const Type* pointer) noexcept { return XENON_BIT_CAST(const std::byte*, pointer); }

	/**
	 * Cast a std::byte pointer to a typed pointer.
	 *
	 * @tparam Type The pointer type.
	 * @param pointer The pointer to cast from.
	 * @return The type pointer.
	 */
	template<class Type>
	XENON_NODISCARD constexpr Type* FromBytes(std::byte* pointer) noexcept { return XENON_BIT_CAST(Type*, pointer); }

	/**
	 * Cast a std::byte pointer to a typed pointer.
	 *
	 * @tparam Type The pointer type.
	 * @param pointer The pointer to cast from.
	 * @return The type pointer.
	 */
	template<class Type>
	XENON_NODISCARD constexpr const Type* FromBytes(const std::byte* pointer) noexcept { return XENON_BIT_CAST(const Type*, pointer); }

	/**
	 * Generate hash for a set of bytes.
	 *
	 * @param pBytes The bytes to hash.
	 * @param size The number of bytes to hash.
	 * @param seed The hash seed. Default is 0.
	 * @return The 64-bit hash value.
	 */
	XENON_NODISCARD uint64_t GenerateHash(const std::byte* pBytes, uint64_t size, uint64_t seed = 0) noexcept;

	/**
	 * Utility function to easily generate the hash for an object.
	 * Note that this will generate the hash for the object memory, not it's content (if using pointers).
	 *
	 * @tparam Type The data type.
	 * @param data The data to generate the hash of.
	 * @param seed The hash seed. Default is 0.
	 * @return The 64-bit hash value.
	 */
	template<class Type>
	XENON_NODISCARD inline uint64_t GenerateHashFor(const Type& data, uint64_t seed = 0) noexcept { return GenerateHash(XENON_BIT_CAST(const std::byte*, &data), sizeof(Type), seed); }
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
	}																						\
																							\
	constexpr name& operator|=(name& lhs, const name rhs)									\
	{																						\
		lhs = lhs | rhs;																	\
		return lhs;																			\
	}
