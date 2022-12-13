// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VertexShader.hpp"

namespace Xenon
{
	namespace ShaderBuilder
	{
		VertexShader::VertexShader()
		{
			m_InstructionStorage.insertOpCapability("OpCapability Shader");
		}
	}
}