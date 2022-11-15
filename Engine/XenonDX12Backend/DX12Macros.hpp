// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonCore/Logging.hpp"

#define XENON_DX12_ASSERT(exp, ...)  XENON_ASSERT(SUCCEEDED(exp), "Direct X 12: " __VA_ARGS__)