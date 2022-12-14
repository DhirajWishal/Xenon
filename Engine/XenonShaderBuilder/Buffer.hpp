// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Uniform.hpp"
#include "Variable.hpp"

#include <unordered_map>

namespace Xenon
{
	namespace ShaderBuilder
	{
		/**
		 * Buffer type.
		 * This is a uniform type which can store (almost) any user defined structure as a buffer.
		 */
		template<class Type>
		class Buffer : public Uniform<Buffer<Type>>
		{
			using Super = Uniform<Buffer<Type>>;

		public:
			/**
			 * Explicit constructor.
			 *
			 * @param storage The assembly storage to record the instructions to.
			 * @param set The descriptor set.
			 * @param binding The uniform binding.
			 */
			template<class... MemberTypes>
			explicit Buffer(AssemblyStorage& storage, uint32_t set, uint32_t binding, const MemberTypes&... members) : Uniform<Buffer<Type>>(storage, set, binding)
			{
				storage.registerObject<Type>("Uniform", members...);
				storage.insertAnnotation(fmt::format("OpDecorate %{} Block", GetTypeIdentifier<Type>()));

				storage.insertType(fmt::format("%uniform_pointer_{} = OpTypePointer Uniform %{}", Super::m_Identifier, GetTypeIdentifier<Type>()));
				storage.insertType(fmt::format("%{} = OpVariable %uniform_pointer_{} Uniform", Super::m_Identifier, Super::m_Identifier));

				// Setup the offset map.
				uint32_t index = 0;
				auto lambda = [this, &storage](const auto& member, uint32_t i) { m_OffsetMap[storage.offsetOf(member)] = i; };
				(lambda(members, index++), ...);
			}

			/**
			 * Access a member from the buffer.
			 *
			 * @tparam ValueType The value type.
			 * @param member The member variable pointer.
			 * @return The value type reference.
			 */
			template<class ValueType>
			[[nodiscard]] Variable<ValueType> access(const ValueType(Type::* member))
			{
				const auto identifier = Super::m_Storage.getUniqueID();
				const auto index = m_OffsetMap[Super::m_Storage.offsetOf(member)];

				Super::m_Storage.storeConstant(index);
				Super::m_Storage.insertFunctionInstruction(fmt::format("%{} = OpAccessChain %member_ptr_{} %{} %{}", identifier, GetTypeIdentifier<ValueType>(), Super::m_Identifier, GetConstantIdentifier(index)));

				return Variable<ValueType>(Super::m_Storage, identifier, static_cast<Type*>(this)->*member);
			}

		private:
			std::unordered_map<std::ptrdiff_t, uint32_t> m_OffsetMap;
		};

		/**
		 * Built-in buffer type.
		 */
		template<class Type>
		class BuiltInBuffer : public DataType
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param storage The assembly storage to record the instructions to.
			 * @param type The buffer type.
			 */
			template<class... MemberTypes>
			explicit BuiltInBuffer(AssemblyStorage& storage, const std::string_view& type, const MemberTypes&... members) : DataType(storage, storage.getUniqueID())
			{
				storage.registerObject<Type>(type, members...);
				storage.insertAnnotation(fmt::format("OpDecorate %{} Block", GetTypeIdentifier<Type>()));

				storage.insertType(fmt::format("%{}_pointer_{} = OpTypePointer {} %{}", type.data(), m_Identifier, type.data(), GetTypeIdentifier<Type>()));
				storage.insertType(fmt::format("%{} = OpVariable %{}_pointer_{} {}", m_Identifier, type.data(), m_Identifier, type.data()));

				// Setup the offset map.
				uint32_t index = 0;
				auto lambda = [this, &storage](const auto& member, uint32_t i) { m_OffsetMap[storage.offsetOf(member)] = i; };
				(lambda(members, index++), ...);
			}

			/**
			 * Access a member from the buffer.
			 *
			 * @tparam ValueType The value type.
			 * @param member The member variable pointer.
			 * @return The value type reference.
			 */
			template<class ValueType>
			[[nodiscard]] Variable<ValueType> access(const ValueType(Type::* member))
			{
				const auto identifier = m_Storage.getUniqueID();
				const auto index = m_OffsetMap[m_Storage.offsetOf(member)];

				m_Storage.storeConstant(index);
				m_Storage.insertFunctionInstruction(fmt::format("%{} = OpAccessChain %member_ptr_{} %{} %{}", identifier, GetTypeIdentifier<ValueType>(), m_Identifier, GetConstantIdentifier(index)));

				return Variable<ValueType>(m_Storage, identifier, static_cast<Type*>(this)->*member);
			}

		private:
			std::unordered_map<std::ptrdiff_t, uint32_t> m_OffsetMap;
		};
	}
}