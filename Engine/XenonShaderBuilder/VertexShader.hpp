// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Builder.hpp"

namespace Xenon
{
	namespace ShaderBuilder
	{
		/**
		 * Vertex shader class.
		 * This can be used to build vertex shaders.
		 */
		class VertexShader final : public Builder
		{
		public:
			/**
			 * Default constructor.
			 */
			VertexShader();

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

				m_InstructionStorage.insertOpEntryPoint(fmt::format("OpEntryPoint Vertex %{} \"{}\"{}", function.getID(), name.data(), attributeString));
			}
		};
	}
}