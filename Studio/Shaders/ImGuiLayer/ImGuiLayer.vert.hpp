// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "E:/Nexonous/Xenon/Engine/XenonBackend/Shader.hpp"

#include <cstdint>

#include "Generated/ImGuiLayer.vert.hlsl.dxil.hpp"
#include "Generated/ImGuiLayer.vert.hlsl.spirv.hpp"

namespace Xenon { namespace Generated { [[nodisacrd]] inline Xenon::Backend::Shader CreateShaderImGuiLayer_vert() { return Xenon::Backend::Shader::Create(g_IMGUILAYER_VERT_SPIRV, g_IMGUILAYER_VERT_DXIL); } } }
