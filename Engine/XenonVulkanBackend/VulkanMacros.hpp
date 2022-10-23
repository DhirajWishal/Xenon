// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonCore/Logging.hpp"

#define XENON_VK_ASSERT(exp, ...)	XENON_ASSERT(exp == VK_SUCCESS, "Vulkan: " __VA_ARGS__)