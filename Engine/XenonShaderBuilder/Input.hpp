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
		 * Input class.
		 * This class contains information about a single input attribute.
		 */
		template<class Type>
		class Input final : public DataType
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param storage The assembly storage to record the instructions to.
			 * @param location The input location.
			 */
			explicit Input(AssemblyStorage& storage, uint32_t location) : DataType(storage), m_Location(location)
			{
				storage.registerType<Type>();
				storage.insertAnnotation(fmt::format("OpDecorate %{} Location {}", m_Identifier, location));
				storage.insertType(fmt::format("%input_{} = OpTypePointer Input {}", m_Identifier, TypeTraits<Type>::Identifier));
				storage.insertType(fmt::format("%{} = OpVariable %input_{} Input", m_Identifier, m_Identifier));
			}

			/**
			 * Implicitly get the stored data variable.
			 *
			 * @return The variable reference.
			 */
			operator const Type& () const noexcept { return m_Variable; }

		private:
			Type m_Variable;
			uint32_t m_Location = 0;
		};
	}
}