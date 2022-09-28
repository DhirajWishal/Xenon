// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <spdlog/spdlog.h>

#define XENON_LOG_INFORMATION(...)			::spdlog::info(__VA_ARGS__)
#define XENON_LOG_WARNING(...)				::spdlog::warn(__VA_ARGS__)
#define XENON_LOG_ERROR(...)				::spdlog::error(__VA_ARGS__)
#define XENON_LOG_FATAL(...)				::spdlog::critical(__VA_ARGS__)

#define XENON_LOG_DEBUG_INFORMATION(...)	SPDLOG_INFO(__VA_ARGS__)
#define XENON_LOG_DEBUG_WARNING(...)		SPDLOG_WARN(__VA_ARGS__)
#define XENON_LOG_DEBUG_ERROR(...)			SPDLOG_ERROR(__VA_ARGS__)
#define XENON_LOG_DEBUG_FATAL(...)			SPDLOG_CRITICAL(__VA_ARGS__)

#define XENON_ASSERT(condition, ...)		if(!(condition))  XENON_LOG_FATAL(__VA_ARGS__)