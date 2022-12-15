// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "TypeTraits.hpp"
#include "Output.hpp"

namespace Xenon
{
	namespace ShaderBuilder
	{
		/**
		 * Variable class.
		 * This class contains information about a single variable.
		 */
		template<class Type>
		class Variable final : public DataType
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param storage The assembly storage to record the instructions to.
			 */
			explicit Variable(AssemblyStorage& storage) : DataType(storage)
			{
				storage.registerType<Type>();
				storage.insertType(fmt::format(FMT_STRING("%variable_type_{} = OpTypePointer Function %{}"), GetTypeIdentifier<Type>(), GetTypeIdentifier<Type>()));
				storage.insertFunctionVariable(fmt::format("%{} = OpVariable %variable_type_{} Function", m_Identifier, GetTypeIdentifier<Type>()));
			}

			/**
			 * Explicit constructor.
			 * This is used by the buffer object to access it's member variables.
			 *
			 * @param storage The assembly storage to record the instructions to.
			 * @param identifier The variable identifier.
			 * @param data The data to initialize.
			 */
			explicit Variable(AssemblyStorage& storage, uint32_t identifier, const Type& data) : DataType(storage, identifier), m_Variable(data)
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
			Type& operator=(const Type& value) { return m_Variable = value; }

			/**
			 * Assign a value to the internal variable.
			 *
			 * @param value The value to assign.
			 * @return The altered value.
			 */
			Type& operator=(const Input<Type>& value)
			{
				const auto loadedID = m_Storage.getUniqueID();
				m_Storage.insertFunctionInstruction(fmt::format("%{} = OpLoad %{} %{}", loadedID, GetTypeIdentifier<Type>(), value.getID()));
				m_Storage.insertFunctionInstruction(fmt::format("OpStore %{} %{}", m_Identifier, loadedID));

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
				const auto loadedID = m_Storage.getUniqueID();
				m_Storage.insertFunctionInstruction(fmt::format("%{} = OpLoad %{} %{}", loadedID, GetTypeIdentifier<Type>(), value.getID()));
				m_Storage.insertFunctionInstruction(fmt::format("OpStore %{} %{}", m_Identifier, loadedID));

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
				const auto loadedID = m_Storage.getUniqueID();
				m_Storage.insertFunctionInstruction(fmt::format("%{} = OpLoad %{} %{}", loadedID, GetTypeIdentifier<Type>(), value.getID()));
				m_Storage.insertFunctionInstruction(fmt::format("OpStore %{} %{}", m_Identifier, loadedID));

				return m_Variable = value;
			}

		private:
			Type m_Variable;
		};
	}
}