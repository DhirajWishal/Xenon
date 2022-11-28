// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <spdlog/spdlog.h>

#include <source_location>

namespace Xenon
{
	/**
	 * Xenon no-op function.
	 * This function compiles down to nothing and the compiler can optimize it out when needed.
	 *
	 * @tparam Arguments The argument types that can be passed to the function.
	 */
	template<class... Arguments>
	constexpr void NoOp(Arguments&&...) noexcept {}

	/**
	 * Print a string of bytes to the console.
	 *
	 * @param pBegin The begin pointer.
	 * @param pEnd The end pointer.
	 */
	void HexDump(const std::byte* pBegin, const std::byte* pEnd);

	/**
	 * Log a trace to the console.
	 *
	 * @tparam Message The message type.
	 * @param location The source location.
	 * @param message The message to log.
	 */
	template<class Message>
	void TraceLog(std::source_location&& location, Message&& message)
	{
		spdlog::info("[Trace \"{}\":{}] {}", location.file_name(), location.line(), std::move(message));
	}
}

/**
 * Xenon log level defines the types of logging that can be done by the engine.
 * The levels are:
 * 1 - Fatal logs only.
 * 2 - Error logs and the log types before this (1).
 * 3 - Warning logs and the log types before this (1, 2).
 * 4 - Information logs and the log types before this (1, 2, 3).
 * 5 - Trace logs and the log types before this (1, 2, 3, 4).
 *
 * If a log level is not defined, no logs will be printed.
 */
#ifdef XENON_LOG_LEVEL
#	if XENON_LOG_LEVEL > 0
#		define XENON_LOG_FATAL(...)								::spdlog::critical(__VA_ARGS__)

#		if XENON_LOG_LEVEL > 1
#			define XENON_LOG_ERROR(...)							::spdlog::error(__VA_ARGS__)

#			if XENON_LOG_LEVEL > 2
#				define XENON_LOG_WARNING(...)					::spdlog::warn(__VA_ARGS__)

#				if XENON_LOG_LEVEL > 3
#					define XENON_LOG_INFORMATION(...)			::spdlog::info(__VA_ARGS__)

#					if XENON_LOG_LEVEL > 4
#						define XENON_LOG_TRACE(msg,...)			::Xenon::TraceLog(std::source_location::current(), fmt::format(msg, __VA_ARGS__))

#					endif
#				endif
#			endif
#		endif
#	endif
#endif // XENON_LOG_LEVEL

#ifndef XENON_LOG_FATAL
#	define XENON_LOG_FATAL(...)									::Xenon::NoOp()
#endif

#ifndef XENON_LOG_ERROR
#	define XENON_LOG_ERROR(...)									::Xenon::NoOp()
#endif

#ifndef XENON_LOG_WARNING
#	define XENON_LOG_WARNING(...)								::Xenon::NoOp()
#endif

#ifndef XENON_LOG_INFORMATION
#	define XENON_LOG_INFORMATION(...)							::Xenon::NoOp()
#endif

#ifndef XENON_LOG_TRACE
#	define XENON_LOG_TRACE(...)									::Xenon::NoOp()
#endif

#ifdef XENON_DEBUG
#	define XENON_LOG_DEBUG(...)									SPDLOG_DEBUG(__VA_ARGS__)

#	ifdef XENON_PLATFORM_WINDOWS
#		define XENON_DEBUG_BREAK								__debugbreak()

#	endif

#else
#	define XENON_LOG_DEBUG(...)									::Xenon::NoOp()
#	define XENON_DEBUG_BREAK									::Xenon::NoOp()

#endif // XENON_DEBUG

#define XENON_ASSERT(condition, ...)							if (!(condition)) (XENON_LOG_FATAL(__VA_ARGS__), XENON_DEBUG_BREAK)

#define XENON_TODO(_day, _month, _year, ...)																											\
	if (std::chrono::year(_year)/_month/_day >= std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())))	\
		XENON_LOG_TRACE("TODO: " __VA_ARGS__)

#define XENON_FIXME(_day, _month, _year, ...)																											\
	if (std::chrono::year(_year)/_month/_day >= std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())))	\
		XENON_LOG_TRACE("FIXME: " __VA_ARGS__)

#define XENON_TODO_NOW(...)										XENON_LOG_TRACE("TODO: " __VA_ARGS__)
#define XENON_FIXME_NOW(...)									XENON_LOG_TRACE("FIXME: " __VA_ARGS__)
