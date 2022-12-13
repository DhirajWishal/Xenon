// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Input.hpp"
#include "Output.hpp"
#include "Function.hpp"

#include "../XenonBackend/Core.hpp"

namespace Xenon
{
	namespace ShaderBuilder
	{
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
		class Builder final
		{
		public:
			/**
			 * Default constructor.
			 */
			Builder() = default;

			/**
			 * Create a new shader input.
			 *
			 * @tparam ValueType The input value type.
			 * @param location The input location.
			 * @return The created input.
			 */
			template<class ValueType>
			[[nodiscard]] Input<ValueType> createInput(uint32_t location) { return Input<ValueType>(m_InstructionStorage, location); }

			/**
			 * Create a new shader output.
			 *
			 * @tparam ValueType The output value type.
			 * @param location The output location.
			 * @return The created output.
			 */
			template<class ValueType>
			[[nodiscard]] Output<ValueType> createOutput(uint32_t location) { return Output<ValueType>(m_InstructionStorage, location); }

			/**
			 * Create a new function.
			 *
			 * @tparam ReturnType The function's return type.
			 * @tparam Parameters The function's parameters.
			 * @return The created builder function.
			 */
			template<class ReturnType, class... Parameters>
			[[nodiscard]] Function<ReturnType, Parameters...> createFunction() { return Function<ReturnType, Parameters...>(m_InstructionStorage); }

			/**
			 * Set an entry point function.
			 *
			 * @tparam ReturnType The return type of the function.
			 * @tparam Attributes The entry point's input and output attribute types.
			 * @param shaderType The type of the shader.
			 * @param name The entry point name.
			 * @param function The entry point function.
			 * @param attributes The input and output attributes used by the entry point.
			 */
			template<class ReturnType, class... Attributes>
			void setEntryPoint(Backend::ShaderType shaderType, std::string&& name, const Function<ReturnType>& function, const Attributes&... attributes)
			{

			}

			/**
			 * Get the instruction storage.
			 *
			 * @return The instruction storage reference.
			 */
			[[nodiscard]] AssemblyStorage& getInstructionStorage() noexcept { return m_InstructionStorage; }

			/**
			 * Get the instruction storage.
			 *
			 * @return The instruction storage reference.
			 */
			[[nodiscard]] const AssemblyStorage& getInstructionStorage() const noexcept { return m_InstructionStorage; }

		private:
			AssemblyStorage m_InstructionStorage;
		};
	}
}