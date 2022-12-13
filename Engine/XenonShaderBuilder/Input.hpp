// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "TypeTraits.hpp"

namespace Xenon
{
	namespace ShaderBuilder
	{
		/**
		 * Input class.
		 * This class contains information about a single input attribute.
		 */
		template<class Type>
		class Input final
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param location The input location.
			 */
			explicit Input(uint32_t location) : m_Location(location) {}

		private:
			uint32_t m_Location = 0;
			Type m_Variable;
		};
	}
}