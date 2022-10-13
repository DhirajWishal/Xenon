// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <spdlog/spdlog.h>

namespace Xenon
{
	/**
	 * Xenon no-op function.
	 * This function compiles down to nothing and the compiler can optimize it out when needed.
	 */
	constexpr void NoOp() {}

	/**
	 * Print a string of bytes to the console.
	 *
	 * @param pBegin The begin pointer.
	 * @param pEnd The end pointer.
	 */
	void HexDump(const std::byte* pBegin, const std::byte* pEnd);
}

#define XENON_LOG_INFORMATION(...)			::spdlog::info(__VA_ARGS__)
#define XENON_LOG_WARNING(...)				::spdlog::warn(__VA_ARGS__)
#define XENON_LOG_ERROR(...)				::spdlog::error(__VA_ARGS__)
#define XENON_LOG_FATAL(...)				::spdlog::critical(__VA_ARGS__)

#ifdef XENON_DEBUG
#define XENON_LOG_DEBUG_INFORMATION(...)	SPDLOG_INFO(__VA_ARGS__)
#define XENON_LOG_DEBUG_WARNING(...)		SPDLOG_WARN(__VA_ARGS__)
#define XENON_LOG_DEBUG_ERROR(...)			SPDLOG_ERROR(__VA_ARGS__)
#define XENON_LOG_DEBUG_FATAL(...)			SPDLOG_CRITICAL(__VA_ARGS__)

#else
#define XENON_LOG_DEBUG_INFORMATION(...)	::Xenon::NoOp()
#define XENON_LOG_DEBUG_WARNING(...)		::Xenon::NoOp()
#define XENON_LOG_DEBUG_ERROR(...)			::Xenon::NoOp()
#define XENON_LOG_DEBUG_FATAL(...)			::Xenon::NoOp()

#endif // XENON_DEBUG

#define XENON_ASSERT(condition, ...)		if(!(condition))  XENON_LOG_FATAL(__VA_ARGS__)