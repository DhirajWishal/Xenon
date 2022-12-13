// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "TypeTraits.hpp"
#include "DataType.hpp"

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
				storage.insertType(fmt::format(FMT_STRING("%variable_type_{} = OpTypePointer Function {}"), TypeTraits<Type>::RawIdentifier, TypeTraits<Type>::Identifier));
				storage.insertFunctionVariable(fmt::format("%{} = OpVariable %variable_type_{} Function", m_Identifier, TypeTraits<Type>::RawIdentifier));
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

		private:
			Type m_Variable;
		};
	}
}