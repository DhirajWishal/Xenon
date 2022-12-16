// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Builder.hpp"
#include "BuiltIn.hpp"
#include "Parameter.hpp"

#include <functional>

namespace Xenon
{
	namespace ShaderBuilder
	{
		/**
		 * Per vertex structure.
		 * This contains the vertex shader's per-vertex outputs.
		 */
		class PerVertexStruct final : public BuiltInBuffer<PerVertexStruct>
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param storage The storage to store the instructions.
			 */
			explicit PerVertexStruct(AssemblyStorage& storage);

		public:
			BuiltIn<glm::vec4> gl_Position;
			BuiltIn<float> gl_PointSize;
			BuiltIn<std::array<float, 1>> gl_ClipDistance;
			BuiltIn<std::array<float, 1>> gl_CullDistance;
		};

		/**
		 * Vertex shader class.
		 * This can be used to build vertex shaders.
		 */
		class VertexShader final : public Builder
		{
		public:
			template<class ReturnType, class... Parameters>
			using FunctionBuilderType = std::function<void(VertexShader&, Function<ReturnType, Parameters...>&, Parameter<Parameters>...)>;

		public:
			/**
			 * Default constructor.
			 */
			VertexShader();

			/**
			 * Create a new function.
			 *
			 * @tparam ReturnType The function's return type.
			 * @tparam Parameters The function's parameters.
			 * @param body The function body used to build the function.
			 * @return The created builder function.
			 */
			template<class ReturnType, class... Parameters>
			[[nodiscard]] Function<ReturnType, Parameters...> createFunction(const FunctionBuilderType<ReturnType, Parameters...>& body)
			{
				auto functionBuilder = Function<ReturnType, Parameters...>(m_InstructionStorage);
				body(*this, functionBuilder, Parameter<Parameters>(m_InstructionStorage)...);

				return functionBuilder;
			}

			/**
			 * Add an entry point function.
			 *
			 * @tparam ReturnType The return type of the function.
			 * @tparam Attributes The entry point's input and output attribute types.
			 * @param name The entry point name.
			 * @param function The entry point function.
			 * @param attributes The input and output attributes used by the entry point.
			 */
			template<class ReturnType, class... Attributes>
			void addEntryPoint(const std::string_view& name, const Function<ReturnType>& function, const Attributes&... attributes)
			{
				std::string attributeString;
				auto lambda = [&attributeString](const auto& attribute) { attributeString += fmt::format(" %{}", attribute.getID()); };
				(lambda(attributes), ...);

				m_InstructionStorage.insertOpEntryPoint(fmt::format("OpEntryPoint Vertex %{} \"{}\" {}", function.getID(), name.data(), attributeString));
			}

		private:
			template<class Type>
			Parameter<Type> createParameter() { return Parameter<Type>(); }

		public:
			PerVertexStruct gl_PerVertex;
		};
	}
}