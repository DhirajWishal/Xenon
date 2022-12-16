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
		 * Parameter type class.
		 * This contains information about a single parameter passed into a function.
		 */
		template<class Type>
		class Parameter final : public DataType
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param storage The assembly storage to record the instructions to.
			 */
			explicit Parameter(AssemblyStorage& storage) : DataType(storage)
			{
				storage.registerType<Type>();
				storage.insertDefinitionOpFunctionParameter(fmt::format("%{} = OpFunctionParameter %variable_type_{}", m_Identifier, GetTypeIdentifier<Type>()));
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

		private:
			Type m_Variable;
		};

		/**
		 * Param alias type.
		 */
		template<class Type>
		using Param = Parameter<Type>;

		/**
		 * Type traits specialization of the parameter class.
		 *
		 * @param T The value type.
		 */
		template<class T>
		struct TypeTraits<Parameter<T>>
		{
			using Type = T;
			static constexpr uint8_t Size = sizeof(Type);
			static constexpr uint8_t ComponentCount = 1;
		};
	}
}