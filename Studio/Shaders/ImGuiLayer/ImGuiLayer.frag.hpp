// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "E:/Nexonous/Xenon/Engine/XenonBackend/Shader.hpp"

#include <cstdint>

#include "Generated/ImGuiLayer.frag.hlsl.dxil.hpp"
#include "Generated/ImGuiLayer.frag.hlsl.spirv.hpp"

namespace Xenon { namespace Generated { [[nodisacrd]] inline Xenon::Backend::Shader CreateShaderImGuiLayer_frag() { return Xenon::Backend::Shader::Create(g_IMGUILAYER_FRAG_SPIRV, g_IMGUILAYER_FRAG_DXIL); } } }
