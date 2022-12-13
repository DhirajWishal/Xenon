// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Input.hpp"
#include "Buffer.hpp"
#include "Output.hpp"
#include "Function.hpp"

#include "../XenonBackend/ShaderSource.hpp"

namespace Xenon
{
	namespace ShaderBuilder
	{
		template<class ValueType>
		class Parameter final {};

		/**
		 * Shader builder class.
		 * This class can be used to build SPIR-V.
		 *
		 * Note that this class is not thread safe and is meant to be written in one thread.
		 */
		class Builder
		{
		public:
			/**
			 * Default constructor.
			 */
			Builder();

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
			 * Create a new uniform buffer.
			 *
			 * @tparam Object The object type.
			 * @param set The descriptor set.
			 * @param binding The uniform binding.
			 * @return The buffer object.
			 */
			template<class Object>
			[[nodiscard]] Object createBuffer(uint32_t set, uint32_t binding) { return Object(m_InstructionStorage, set, binding); }

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

			/**
			 * Generate the shader source using the recorded assembly.
			 *
			 * @return The generated shader source.
			 */
			[[nodiscard]] Backend::ShaderSource generate() const;

		protected:
			AssemblyStorage m_InstructionStorage;
		};
	}
}