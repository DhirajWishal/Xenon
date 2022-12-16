// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Variable.hpp"

namespace Xenon
{
	namespace ShaderBuilder
	{
		/**
		 * Function class.
		 * This class acts as a single function in SPIR-V and is used to create variables, perform logic operations and other things we do in functions.
		 */
		template<class ReturnType, class... Parameters>
		class Function final : public DataType
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param storage The instruction storage.
			 */
			explicit Function(AssemblyStorage& storage) : DataType(storage)
			{
				storage.registerCallable<ReturnType, Parameters...>();

				storage.beginFunctionDefinition();
				storage.setDefinitionOpFunction(fmt::format("%{} = OpFunction %{} None %{}", m_Identifier, GetTypeIdentifier<ReturnType>(), storage.getFunctionIdentifier<ReturnType, Parameters...>()));
			}

			/**
			 * Exit the function by returning a value.
			 * 
			 * @param value The value to return.
			 */
			void exit(const Input<ReturnType>& value)
			{
				const auto loadedID = m_Storage.getUniqueID();
				m_Storage.insertFunctionInstruction(fmt::format("%{} = OpLoad %{} %{}", loadedID, GetTypeIdentifier<ReturnType>(), value.getID()));
				m_Storage.setFunctionOpReturn(fmt::format("OpReturnValue %{}", loadedID));
			}

			/**
			 * Exit the function by returning a value.
			 *
			 * @param value The value to return.
			 */
			void exit(const Output<ReturnType>& value)
			{
				const auto loadedID = m_Storage.getUniqueID();
				m_Storage.insertFunctionInstruction(fmt::format("%{} = OpLoad %{} %{}", loadedID, GetTypeIdentifier<ReturnType>(), value.getID()));
				m_Storage.setFunctionOpReturn(fmt::format("OpReturnValue %{}", loadedID));
			}

			/**
			 * Exit the function by returning a value.
			 *
			 * @param value The value to return.
			 */
			void exit(const Parameter<ReturnType>& value)
			{
				const auto loadedID = m_Storage.getUniqueID();
				m_Storage.insertFunctionInstruction(fmt::format("%{} = OpLoad %{} %{}", loadedID, GetTypeIdentifier<ReturnType>(), value.getID()));
				m_Storage.setFunctionOpReturn(fmt::format("OpReturnValue %{}", loadedID));
			}

			/**
			 * Exit the function by returning a value.
			 *
			 * @param value The value to return.
			 */
			void exit(const Variable<ReturnType>& value)
			{
				const auto loadedID = m_Storage.getUniqueID();
				m_Storage.insertFunctionInstruction(fmt::format("%{} = OpLoad %{} %{}", loadedID, GetTypeIdentifier<ReturnType>(), value.getID()));
				m_Storage.setFunctionOpReturn(fmt::format("OpReturnValue %{}", loadedID));
			}

			/**
			 * Create a new variable.
			 *
			 * @tparam ValueType The variable's value type.
			 * @return The variable.
			 */
			template<class ValueType>
			[[nodiscard]] Variable<ValueType> createVariable() { return Variable<ValueType>(m_Storage); }

			template<class ValueType>
			Variable<ValueType> createVariable(ValueType) {}

			template<class ValueType>
			Variable<ValueType> createVariable(Input<ValueType>) {}

			template<class ValueType>
			Variable<ValueType> createVariable(Variable<ValueType>) {}
		};

		/**
		 * Function class specialization.
		 * This class acts as a single function in SPIR-V and is used to create variables, perform logic operations and other things we do in functions.
		 */
		template<class... Parameters>
		class Function<void, Parameters...> final : public DataType
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param storage The instruction storage.
			 */
			explicit Function(AssemblyStorage& storage) : DataType(storage)
			{
				storage.registerCallable<void, Parameters...>();
				// storage.insertExecutionMode(fmt::format("OpExecutionMode %{} OriginLowerLeft", m_Identifier)); Fragment only!

				storage.beginFunctionDefinition();
				storage.setDefinitionOpFunction(fmt::format("%{} = OpFunction %{} None %{}", m_Identifier, GetTypeIdentifier<void>(), storage.getFunctionIdentifier<void, Parameters...>()));
				storage.setFunctionOpReturn("OpReturn");
			}

			/**
			 * Create a new variable.
			 *
			 * @tparam ValueType The variable's value type.
			 * @return The variable.
			 */
			template<class ValueType>
			[[nodiscard]] Variable<ValueType> createVariable() { return Variable<ValueType>(m_Storage); }

			template<class ValueType>
			Variable<ValueType> createVariable(ValueType) {}

			template<class ValueType>
			Variable<ValueType> createVariable(Input<ValueType>) {}

			template<class ValueType>
			Variable<ValueType> createVariable(Variable<ValueType>) {}
		};
	}
}