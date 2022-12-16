// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "TypeTraits.hpp"
#include "Input.hpp"

namespace Xenon
{
	namespace ShaderBuilder
	{
		/**
		 * Output class.
		 * This class contains information about a single output attribute.
		 */
		template<class Type>
		class Output final : public DataType
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param storage The assembly storage to record the instructions to.
			 * @param location The output location.
			 */
			explicit Output(AssemblyStorage& storage, uint32_t location) : DataType(storage), m_Location(location)
			{
				storage.registerType<Type>();
				storage.insertAnnotation(fmt::format("OpDecorate %{} Location {}", m_Identifier, location));
				storage.insertType(fmt::format("%output_{} = OpTypePointer Output %{}", m_Identifier, GetTypeIdentifier<Type>()));
				storage.insertType(fmt::format("%{} = OpVariable %output_{} Output", m_Identifier, m_Identifier));
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
				m_Storage.insertFunctionInstruction(fmt::format("OpCopyMemory %{} %{}", m_Identifier, value.getID()));
				return m_Variable = value;
			}

		private:
			Type m_Variable;
			uint32_t m_Location = 0;
		};

		/**
		 * Out alias type.
		 */
		template<class Type>
		using Out = Output<Type>;

		/**
		 * Type traits specialization of the output class.
		 *
		 * @param T The value type.
		 */
		template<class T>
		struct TypeTraits<Output<T>>
		{
			using Type = T;
			static constexpr uint8_t Size = sizeof(Type);
			static constexpr uint8_t ComponentCount = 1;
		};
	}
}