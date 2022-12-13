// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <glm/glm.hpp>

namespace Xenon
{
	template<class ValueType>
	class Input final {};

	template<class ValueType>
	class Output final {};

	template<class ValueType>
	class Variable final {};

	template<class ValueType>
	class Parameter final {};

	template<class Type>
	class Uniform
	{
	public:
		template<class Member>
		Member& access(Member)
		{

		}
	};

	/**
	 * Shader builder class.
	 * This class can be used to build SPIR-V.
	 *
	 * Note that this class is not thread safe and is meant to be written in one thread.
	 */
	class ShaderBuilder final
	{
	public:
		/**
		 * Default constructor.
		 */
		ShaderBuilder() = default;

		template<class ValueType>
		Input<ValueType> createInput(uint32_t location) {}

		template<class ValueType>
		Output<ValueType> createOutput(uint32_t location) {}

	private:
		[[nodiscard]] uint32_t getUniqueIdentifier() noexcept { return m_UniqueIdentifier++; }

	private:
		uint32_t m_UniqueIdentifier = 1;
	};

	class FunctionBuilder final
	{
	public:
		template<class ValueType>
		Variable<ValueType> createVariable() {}

		template<class ValueType>
		Variable<ValueType> createVariable(ValueType) {}

		template<class ValueType>
		Variable<ValueType> createVariable(Input<ValueType>) {}

		template<class ValueType>
		Variable<ValueType> createVariable(Variable<ValueType>) {}
	};
}