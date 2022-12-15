// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Input.hpp"
#include "Output.hpp"
#include "Variable.hpp"

namespace Xenon
{
	namespace ShaderBuilder
	{
		/**
		 * Built in type class.
		 * This class stores information regarding a single built-in type.
		 */
		template<class Type>
		class BuiltIn final : public DataType
		{
		public:
			/**
			 * Explicit constructor.
			 * This is used by the buffer object to access it's member variables.
			 *
			 * @param storage The assembly storage to record the instructions to.
			 * @param identifier The variable identifier.
			 */
			explicit BuiltIn(AssemblyStorage& storage, uint32_t identifier) : DataType(storage, identifier)
			{
				storage.registerType<Type>();
			}

			/**
			 * Implicitly get the stored data variable.
			 *
			 * @return The variable reference.
			 */
			operator Type& () noexcept { return m_Variable; }

			/**
			 * Implicitly get the stored data variable.
			 *
			 * @return The variable reference.
			 */
			operator const Type& () const noexcept { return m_Variable; }

			/**
			 * Assign a value to the internal variable.
			 *
			 * @param value The value to assign.
			 * @return The altered value.
			 */
			Type& operator=(const Input<Type>& value)
			{
				const auto temporary = m_Storage.getUniqueID();
				m_Storage.insertFunctionInstruction(fmt::format("%{} = OpLoad %{} %{}", temporary, GetTypeIdentifier<Type>(), value.getID()));
				m_Storage.insertFunctionInstruction(fmt::format("OpCopyMemory %{} %{}", m_Identifier, temporary));

				return m_Variable = value;
			}

			/**
			 * Assign a value to the internal variable.
			 *
			 * @param value The value to assign.
			 * @return The altered value.
			 */
			Type& operator=(const Output<Type>& value)
			{
				const auto temporary = m_Storage.getUniqueID();
				m_Storage.insertFunctionInstruction(fmt::format("%{} = OpLoad %{} %{}", temporary, GetTypeIdentifier<Type>(), value.getID()));
				m_Storage.insertFunctionInstruction(fmt::format("OpCopyMemory %{} %{}", m_Identifier, temporary));

				return m_Variable = value;
			}

			/**
			 * Assign a value to the internal variable.
			 *
			 * @param value The value to assign.
			 * @return The altered value.
			 */
			Type& operator=(const Variable<Type>& value)
			{
				const auto temporary = m_Storage.getUniqueID();
				m_Storage.insertFunctionInstruction(fmt::format("%{} = OpLoad %{} %{}", temporary, GetTypeIdentifier<Type>(), value.getID()));
				m_Storage.insertFunctionInstruction(fmt::format("OpCopyMemory %{} %{}", m_Identifier, temporary));


				return m_Variable = value;
			}

		private:
			Type m_Variable;
		};

		template<class T>
		struct TypeTraits<BuiltIn<T>>
		{
			using Type = T;
			static constexpr uint8_t Size = sizeof(Type);
			static constexpr uint8_t ComponentCount = 1;
		};
	}
}