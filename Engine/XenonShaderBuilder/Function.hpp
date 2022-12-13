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
				// storage.insertExecutionMode(fmt::format("OpExecutionMode %{} OriginLowerLeft", m_Identifier)); Fragment only!

				storage.beginFunctionDefinition();
				storage.setDefinitionOpFunction(fmt::format("%{} = OpFunction {} None {}", m_Identifier, TypeTraits<ReturnType>::Identifier, storage.getFunctionIdentifier<ReturnType, Parameters...>()));

				if constexpr (std::is_void_v<ReturnType>)
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