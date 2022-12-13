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
		 * Uniform class.
		 * This class contains information about a single uniform.
		 */
		template<class Type>
		class Uniform : public DataType
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param storage The assembly storage to record the instructions to.
			 * @param set The descriptor set.
			 * @param binding The uniform binding.
			 */
			explicit Uniform(AssemblyStorage& storage, uint32_t set, uint32_t binding) : DataType(storage), m_Set(set), m_Binding(binding)
			{
				storage.insertAnnotation(fmt::format("OpDecorate %{} DescriptorSet {}", m_Identifier, set));
				storage.insertAnnotation(fmt::format("OpDecorate %{} Binding {}", m_Identifier, binding));
			}

		private:
			uint32_t m_Set = 0;
			uint32_t m_Binding = 0;
		};
	}
}