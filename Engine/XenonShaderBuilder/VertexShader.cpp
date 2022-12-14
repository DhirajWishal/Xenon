// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VertexShader.hpp"

namespace Xenon
{
	namespace ShaderBuilder
	{
		PerVertexStruct::PerVertexStruct(AssemblyStorage& storage)
			: BuiltInBuffer<PerVertexStruct>(storage, "Output", &PerVertexStruct::gl_Position, &PerVertexStruct::gl_PointSize, &PerVertexStruct::gl_ClipDistance, &PerVertexStruct::gl_CullDistance)
		{
			const auto structIdentifier = GetTypeIdentifier<PerVertexStruct>();

			// Setup the annotations.
			storage.insertAnnotation(fmt::format("OpMemberDecorate %{} 0 BuiltIn Position", structIdentifier));
			storage.insertAnnotation(fmt::format("OpMemberDecorate %{} 1 BuiltIn PointSize", structIdentifier));
			storage.insertAnnotation(fmt::format("OpMemberDecorate %{} 2 BuiltIn ClipDistance", structIdentifier));
			storage.insertAnnotation(fmt::format("OpMemberDecorate %{} 3 BuiltIn CullDistance", structIdentifier));

			// Setup the names.
			storage.insertName(fmt::format("OpName %{} \"gl_PerVertex\"", structIdentifier));
			storage.insertName(fmt::format("OpMemberName %{} 0 \"gl_Position\"", structIdentifier));
			storage.insertName(fmt::format("OpMemberName %{} 1 \"gl_PointSize\"", structIdentifier));
			storage.insertName(fmt::format("OpMemberName %{} 2 \"gl_ClipDistance\"", structIdentifier));
			storage.insertName(fmt::format("OpMemberName %{} 3 \"gl_CullDistance\"", structIdentifier));

			// Setup constants.
			storage.storeConstant(0);
			storage.storeConstant(1);
		}

		VertexShader::VertexShader()
			: gl_PerVertex(m_InstructionStorage)
		{
			m_InstructionStorage.insertOpCapability("OpCapability Shader");
			m_InstructionStorage.insertDebugSource("OpSource GLSL 450");
		}
	}
}