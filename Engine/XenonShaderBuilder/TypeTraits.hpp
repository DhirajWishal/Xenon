// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <glm/glm.hpp>

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
			static constexpr const char RawIdentifier[] = "void";
			static constexpr const char Identifier[] = "%void";
			static constexpr const char Declaration[] = "OpTypeVoid";
			static constexpr uint8_t Size = 0;
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
			static constexpr const char RawIdentifier[] = "bool";
			static constexpr const char Identifier[] = "%bool";
			static constexpr const char Declaration[] = "OpTypeBool";
			static constexpr uint8_t Size = sizeof(Type);
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
			static constexpr const char RawIdentifier[] = "int8";
			static constexpr const char Identifier[] = "%int8";
			static constexpr const char Declaration[] = "OpTypeInt 8 1";
			static constexpr uint8_t Size = sizeof(Type);
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
			static constexpr const char RawIdentifier[] = "uint8";
			static constexpr const char Identifier[] = "%uint8";
			static constexpr const char Declaration[] = "OpTypeInt 8 0";
			static constexpr uint8_t Size = sizeof(Type);
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
			static constexpr const char RawIdentifier[] = "int16";
			static constexpr const char Identifier[] = "%int16";
			static constexpr const char Declaration[] = "OpTypeInt 16 1";
			static constexpr uint8_t Size = sizeof(Type);
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
			static constexpr const char RawIdentifier[] = "uint16";
			static constexpr const char Identifier[] = "%uint16";
			static constexpr const char Declaration[] = "OpTypeInt 16 0";
			static constexpr uint8_t Size = sizeof(Type);
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
			static constexpr const char RawIdentifier[] = "int32";
			static constexpr const char Identifier[] = "%int32";
			static constexpr const char Declaration[] = "OpTypeInt 32 1";
			static constexpr uint8_t Size = sizeof(Type);
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
			static constexpr const char RawIdentifier[] = "uint32";
			static constexpr const char Identifier[] = "%uint32";
			static constexpr const char Declaration[] = "OpTypeInt 32 0";
			static constexpr uint8_t Size = sizeof(Type);
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
			static constexpr const char RawIdentifier[] = "int64";
			static constexpr const char Identifier[] = "%int64";
			static constexpr const char Declaration[] = "OpTypeInt 64 1";
			static constexpr uint8_t Size = sizeof(Type);
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
			static constexpr const char RawIdentifier[] = "uint64";
			static constexpr const char Identifier[] = "%uint64";
			static constexpr const char Declaration[] = "OpTypeInt 64 0";
			static constexpr uint8_t Size = sizeof(Type);
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
			static constexpr const char RawIdentifier[] = "float";
			static constexpr const char Identifier[] = "%float";
			static constexpr const char Declaration[] = "OpTypeFloat 32";
			static constexpr uint8_t Size = sizeof(Type);
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
			static constexpr const char RawIdentifier[] = "double";
			static constexpr const char Identifier[] = "%double";
			static constexpr const char Declaration[] = "OpTypeFloat 64";
			static constexpr uint8_t Size = sizeof(Type);
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
			static constexpr const char RawIdentifier[] = "vec2";
			static constexpr const char Identifier[] = "%vec2";
			static constexpr const char Declaration[] = "OpTypeVector %float 2";
			static constexpr uint8_t Size = sizeof(Type);
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
			static constexpr const char RawIdentifier[] = "vec3";
			static constexpr const char Identifier[] = "%vec3";
			static constexpr const char Declaration[] = "OpTypeVector %float 3";
			static constexpr uint8_t Size = sizeof(Type);
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
			static constexpr const char RawIdentifier[] = "vec4";
			static constexpr const char Identifier[] = "%vec4";
			static constexpr const char Declaration[] = "OpTypeVector %float 4";
			static constexpr uint8_t Size = sizeof(Type);
			static constexpr uint8_t ComponentCount = 2;
		};
	}
}