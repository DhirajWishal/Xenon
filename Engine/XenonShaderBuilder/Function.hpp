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

			/**
			 * Function call operator.
			 * This accepts any argument type and will internally convert to a variable.
			 *
			 * @tparam Arguments The incoming argument types.
			 * @param arguments The arguments.
			 * @return The return of the function.
			 */
			template<class... Arguments>
			Variable<ReturnType> operator()(const Arguments&... arguments)
			{
				return operator()(toArgument(arguments)...);
			}

			/**
			 * Function call operator.
			 *
			 * @tparam Arguments The incoming argument types.
			 * @param arguments The arguments to call the function with.
			 * @return The return variable.
			 */
			template<class... Arguments>
			Variable<ReturnType> operator()(const Variable<Arguments>&... arguments)
			{
				std::string argumentString;
				auto lambda = [&argumentString](const auto& argument) { argumentString += fmt::format(" %{}", argument.getID()); };
				(lambda(arguments), ...);

				const auto resultID = m_Storage.getUniqueID();
				m_Storage.insertFunctionInstruction(fmt::format("%{} = OpFunctionCall %{} %{} {}", resultID, GetTypeIdentifier<ReturnType>(), m_Identifier, argumentString));

				return Variable<ReturnType>(m_Storage, resultID, {});
			}

		private:
			/**
			 * Convert an input type to an acceptable variable type.
			 *
			 * @tparam Type The value type.
			 * @param input The input to convert to a variable.
			 * @return The variable.
			 */
			template<class Type>
			Variable<Type> toArgument(const Input<Type>& input)
			{
				auto var = Variable<Type>(m_Storage);
				var = input;
				return var;
			}

			/**
			 * Convert an output type to an acceptable variable type.
			 *
			 * @tparam Type The value type.
			 * @param output The output to convert to a variable.
			 * @return The variable.
			 */
			template<class Type>
			Variable<Type> toArgument(const Output<Type>& output)
			{
				auto var = Variable<Type>(m_Storage);
				var = output;
				return var;
			}

			/**
			 * Convert an variable type to an acceptable variable type.
			 * This will not create a new variable, rather return the incoming one.
			 *
			 * @tparam Type The value type.
			 * @param variable The variable to convert to a variable.
			 * @return The variable.
			 */
			template<class Type>
			Variable<Type> toArgument(const Variable<Type>& variable)
			{
				return variable;
			}
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

			/**
			 * Function call operator.
			 * This accepts any argument type and will internally convert to a variable.
			 *
			 * @tparam Arguments The incoming argument types.
			 * @param arguments The arguments.
			 */
			template<class... Arguments>
			void operator()(const Arguments&... arguments)
			{
				operator()(toArgument(arguments)...);
			}

			/**
			 * Function call operator.
			 *
			 * @tparam Arguments The incoming argument types.
			 * @param arguments The arguments to call the function with.
			 */
			template<class... Arguments>
			void operator()(const Variable<Arguments>&... arguments)
			{
				std::string argumentString;
				auto lambda = [&argumentString](const auto& argument) { argumentString += fmt::format(" %{}", argument.getID()); };
				(lambda(arguments), ...);

				const auto resultID = m_Storage.getUniqueID();
				m_Storage.insertFunctionInstruction(fmt::format("%{} = OpFunctionCall %{} %{} {}", resultID, GetTypeIdentifier<void>(), m_Identifier, argumentString));
			}

		private:
			/**
			 * Convert an input type to an acceptable variable type.
			 *
			 * @tparam Type The value type.
			 * @param input The input to convert to a variable.
			 * @return The variable.
			 */
			template<class Type>
			Variable<Type> toArgument(const Input<Type>& input)
			{
				auto var = Variable<Type>(m_Storage);
				var = input;
				return var;
			}

			/**
			 * Convert an output type to an acceptable variable type.
			 *
			 * @tparam Type The value type.
			 * @param output The output to convert to a variable.
			 * @return The variable.
			 */
			template<class Type>
			Variable<Type> toArgument(const Output<Type>& output)
			{
				auto var = Variable<Type>(m_Storage);
				var = output;
				return var;
			}

			/**
			 * Convert an variable type to an acceptable variable type.
			 * This will not create a new variable, rather return the incoming one.
			 *
			 * @tparam Type The value type.
			 * @param variable The variable to convert to a variable.
			 * @return The variable.
			 */
			template<class Type>
			Variable<Type> toArgument(const Variable<Type>& variable)
			{
				return variable;
			}
		};
	}
}