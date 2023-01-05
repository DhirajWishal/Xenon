// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonCore/Logging.hpp"

#define XENON_VK_ASSERT(exp, ...)					XENON_ASSERT(exp == VK_SUCCESS, "Vulkan: " __VA_ARGS__)
#define XENON_VK_BOOL(value)						value ? VK_TRUE : VK_FALSE
#define XENON_VK_ALIGNED_SIZE(size, alignment)		((size + alignment - 1) & ~(alignment - 1))