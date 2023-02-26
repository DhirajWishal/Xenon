// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <spdlog/spdlog.h>

constexpr auto g_XenonLoggerName = "XenonStudio";

/**
 * Xenon studio logger.
 * Compared to the Xenon logger, this will log everything to the studio's logger instead.
 *
 * @param level The log level.
 * @param VA ARGS The log message to send.
 */
#define XENON_STUDIO_LOG(level, ...)								::spdlog::get("XenonStudio")->log(level, __VA_ARGS__)

#define XENON_STUDIO_LOG_FATAL(...)									XENON_STUDIO_LOG(::spdlog::level::critical, __VA_ARGS__)
#define XENON_STUDIO_LOG_ERROR(...)									XENON_STUDIO_LOG(::spdlog::level::err, __VA_ARGS__)
#define XENON_STUDIO_LOG_WARNING(...)								XENON_STUDIO_LOG(::spdlog::level::warn, __VA_ARGS__)
#define XENON_STUDIO_LOG_INFORMATION(...)							XENON_STUDIO_LOG(::spdlog::level::info, __VA_ARGS__)