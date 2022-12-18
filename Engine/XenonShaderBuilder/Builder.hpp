// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Input.hpp"
#include "Buffer.hpp"
#include "Output.hpp"
#include "Function.hpp"

#include "../XenonBackend/ShaderSource.hpp"
#include "../XenonCore/Logging.hpp"

namespace Xenon
{
	namespace ShaderBuilder
	{
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
			 * Create a new uniform buffer.
			 *
			 * @tparam Object The object type.
			 * @param set The descriptor set.
			 * @param binding The uniform binding.
			 * @return The buffer object.
			 */
			template<class Object>
			[[nodiscard]] Object createBuffer(uint32_t set, uint32_t binding) { return Object(m_InstructionStorage, set, binding); }

			template<class ValueType, class... Arguments>
			[[nodiscard]] Variable<ValueType> createVariable(Arguments... arguments)
			{
				(setupArgument(arguments), ...);
				// %float_0 = OpConstant %float 0
				// %float_1 = OpConstant %float 1
				//      %54 = OpCompositeExtract %float %51 0
				//	    %55 = OpCompositeExtract %float %51 1
				//	    %56 = OpCompositeConstruct %v4float %54 %55 %float_0 %float_1

				return Variable<ValueType>(m_InstructionStorage);
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

			/**
			 * Generate the shader source using the recorded assembly.
			 *
			 * @return The generated shader source.
			 */
			[[nodiscard]] Backend::ShaderSource generate() const;

		private:
			template<class Type>
			void setupArgument(Type argument)
			{
				if constexpr (std::is_reference_v<Type>)
					XENON_LOG_INFORMATION("Yup, definitely a reference type!");

				else if constexpr (std::is_rvalue_reference_v<Type>)
					XENON_LOG_INFORMATION("Yup, definitely a literal!");

				else if constexpr (std::is_lvalue_reference_v<Type>)
					XENON_LOG_INFORMATION("A variable maybe?");

				else
					XENON_LOG_INFORMATION("IDK what the hell this is...");
			}

		protected:
			AssemblyStorage m_InstructionStorage;
		};
	}
}