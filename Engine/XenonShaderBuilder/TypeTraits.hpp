// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <glm/glm.hpp>
#include <spdlog/fmt/fmt.h>

#include <concepts>

namespace Xenon
{
	namespace ShaderBuilder
	{
		/**
		 * Type traits structure.
		 * This is the generalization structure and the specializations are used to store type-specific information.
		 *
		 * @tparam Type The type to contain the traits of.
		 */
		template<class Type>
		struct TypeTraits;

		/**
		 * Void type traits.
		 */
		template <>
		struct TypeTraits<void>
		{
			using Type = void;
			using ComponentType = Type;
			static constexpr uint8_t ComponentCount = 1;
		};

		/**
		 * Boolean type traits.
		 */
		template <>
		struct TypeTraits<bool>
		{
			using Type = bool;
			using ComponentType = Type;
			static constexpr uint8_t ComponentCount = 1;
		};

		/**
		 * 8 bit signed integer type traits.
		 */
		template <>
		struct TypeTraits<int8_t>
		{
			using Type = int8_t;
			using ComponentType = Type;
			static constexpr uint8_t ComponentCount = 1;
		};

		/**
		 * 8 bit unsigned integer type traits.
		 */
		template <>
		struct TypeTraits<uint8_t>
		{
			using Type = uint8_t;
			using ComponentType = Type;
			static constexpr uint8_t ComponentCount = 1;
		};

		/**
		 * 16 bit signed integer type traits.
		 */
		template <>
		struct TypeTraits<int16_t>
		{
			using Type = int16_t;
			using ComponentType = Type;
			static constexpr uint8_t ComponentCount = 1;
		};

		/**
		 * 16 bit unsigned integer type traits.
		 */
		template <>
		struct TypeTraits<uint16_t>
		{
			using Type = uint16_t;
			using ComponentType = Type;
			static constexpr uint8_t ComponentCount = 1;
		};

		/**
		 * 32 bit signed integer type traits.
		 */
		template <>
		struct TypeTraits<int32_t>
		{
			using Type = int32_t;
			using ComponentType = Type;
			static constexpr uint8_t ComponentCount = 1;
		};

		/**
		 * 32 bit unsigned integer type traits.
		 */
		template <>
		struct TypeTraits<uint32_t>
		{
			using Type = uint32_t;
			using ComponentType = Type;
			static constexpr uint8_t ComponentCount = 1;
		};

		/**
		 * 64 bit signed integer type traits.
		 */
		template <>
		struct TypeTraits<int64_t>
		{
			using Type = int64_t;
			using ComponentType = Type;
			static constexpr uint8_t ComponentCount = 1;
		};

		/**
		 * 64 bit unsigned integer type traits.
		 */
		template <>
		struct TypeTraits<uint64_t>
		{
			using Type = uint64_t;
			using ComponentType = Type;
			static constexpr uint8_t ComponentCount = 1;
		};

		/**
		 * Float type traits.
		 */
		template <>
		struct TypeTraits<float>
		{
			using Type = float;
			using ComponentType = Type;
			static constexpr uint8_t ComponentCount = 1;
		};

		/**
		 * Double type traits.
		 */
		template <>
		struct TypeTraits<double>
		{
			using Type = double;
			using ComponentType = Type;
			static constexpr uint8_t ComponentCount = 1;
		};

		/**
		 * GLM vector 2 type traits.
		 */
		template <>
		struct TypeTraits<glm::vec2>
		{
			using Type = glm::vec2;
			using ComponentType = Type::value_type;
			static constexpr uint8_t ComponentCount = 2;
		};

		/**
		 * GLM vector 3 type traits.
		 */
		template <>
		struct TypeTraits<glm::vec3>
		{
			using Type = glm::vec3;
			using ComponentType = Type::value_type;
			static constexpr uint8_t ComponentCount = 2;
		};

		/**
		 * GLM vector 4 type traits.
		 */
		template <>
		struct TypeTraits<glm::vec4>
		{
			using Type = glm::vec4;
			using ComponentType = Type::value_type;
			static constexpr uint8_t ComponentCount = 2;
		};

		/**
		 * Is std::array<,> generalized boolean.
		 * This will be set to false if the type is not a std::array<,> type.
		 */
		template<class... Types>
		constexpr bool IsStdArray = false;

		/**
		 * Is std::array<,> specialized boolean.
		 * This will be set to true if the type is a std::array<,> type.
		 */
		template<class Type, size_t Size>
		constexpr bool IsStdArray<std::array<Type, Size>> = true;

		/**
		 * Get a type's identifier.
		 * The type identifiers are the same as the typeid hash of a type.
		 *
		 * @tparam Type The type to get the identifier of.
		 * @return The type identifier string. Note that you explicitly need to set the % when using it.
		 */
		template<class Type>
		[[nodiscard]] constexpr std::string GetTypeIdentifier() noexcept { return fmt::to_string(typeid(Type).hash_code()); }

		/**
		 * Get the constant value's identifier.
		 * Make sure that the type is registered.
		 *
		 * @tparam Type The type of the value.
		 * @param value The constant value.
		 * @return The identifier string.
		 */
		template<class Type>
		[[nodiscard]] constexpr std::string GetConstantIdentifier(const Type& value) { return fmt::format("const_{}_{}", GetTypeIdentifier<Type>(), value); }

		/**
		 * Get type declaration function general type.
		 *
		 * @tparam Type The type to get the declaration of.
		 */
		template<class Type>
		[[nodiscard]] constexpr std::string GetTypeDeclaration() { return ""; };

		/**
		 * Get the void type declaration.
		 *
		 * @return The type declaration.
		 */
		template<>
		[[nodiscard]] constexpr std::string GetTypeDeclaration<void>() { return "OpTypeVoid"; }

		/**
		 * Get the bool type declaration.
		 *
		 * @return The type declaration.
		 */
		template<>
		[[nodiscard]] constexpr std::string GetTypeDeclaration<bool>() { return "OpTypeBool"; }

		/**
		 * Get the unsigned integer type declaration.
		 *
		 * @return The type declaration.
		 */
		template<std::unsigned_integral Type>
		[[nodiscard]] constexpr std::string GetTypeDeclaration() { return fmt::format("OpTypeInt {} 0", sizeof(Type) * 8); };

		/**
		 * Get the signed integer type declaration.
		 *
		 * @return The type declaration.
		 */
		template<std::integral Type>
		[[nodiscard]] constexpr std::string GetTypeDeclaration() { return fmt::format("OpTypeInt {} 1", sizeof(Type) * 8); };

		/**
		 * Get the float type declaration.
		 *
		 * @return The type declaration.
		 */
		template<>
		[[nodiscard]] constexpr std::string GetTypeDeclaration<float>() { return "OpTypeFloat 32"; }

		/**
		 * Get the double type declaration.
		 *
		 * @return The type declaration.
		 */
		template<>
		[[nodiscard]] constexpr std::string GetTypeDeclaration<double>() { return "OpTypeFloat 64"; }

		/**
		 * Vector traits generalized structure.
		 *
		 * @param T The type.
		 */
		template<class T>
		struct VectorTraits;

		/**
		 * Vector traits specialization structure.
		 *
		 * @param C The vector's component count.
		 * @param T The type.
		 */
		template<size_t C, class T>
		struct VectorTraits<glm::vec<C, T, glm::qualifier::defaultp>>
		{
			using ValueType = T;
			static constexpr size_t Components = C;
		};

		/**
		 * glm::vec<,,> concept.
		 * This will only allow glm::vec types.
		 */
		template<class Type>
		concept GLMVector = requires(Type type) { [] <size_t C, class T>(glm::vec<C, T, glm::qualifier::defaultp>&) {}(type); };

		/**
		 * Get the GLM vector type declaration.
		 *
		 * @return The type declaration.
		 */
		template<GLMVector Type>
		[[nodiscard]] constexpr std::string GetTypeDeclaration() { return fmt::format("OpTypeVector %{} {}", GetTypeIdentifier<typename VectorTraits<Type>::ValueType>(), VectorTraits<Type>::Components); }

		/**
		 * Array traits generalized structure.
		 *
		 * @param T The type.
		 */
		template<class T>
		struct ArrayTraits;

		/**
		 * Array traits specialization structure.
		 *
		 * @param T The type.
		 * @param S The array's size.
		 */
		template<class T, size_t S>
		struct ArrayTraits<std::array<T, S>>
		{
			using ValueType = T;
			static constexpr size_t Size = S;
		};

		/**
		 * std::array<,> concept.
		 * This will only allow std::array types.
		 */
		template<class Type>
		concept StdArray = requires(Type type) { [] <class T, size_t C>(std::array<T, C>&) {}(type); };

		/**
		 * Get the std::array<,> type declaration.
		 *
		 * @return The type declaration.
		 */
		template<StdArray Type>
		[[nodiscard]] constexpr std::string GetTypeDeclaration() { return fmt::format("OpTypeArray %{} %{}", GetTypeIdentifier<typename ArrayTraits<Type>::ValueType>(), GetConstantIdentifier(ArrayTraits<Type>::Size)); }
	}
}