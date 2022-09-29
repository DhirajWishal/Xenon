// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonCore/Logging.hpp"

#define XENON_DX12_ASSERT(exp, ...) if(FAILED(exp)) XENON_LOG_FATAL(__VA_ARGS__)