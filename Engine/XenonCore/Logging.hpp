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
	 */
	constexpr void NoOp() {}

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
	 * @tparam Arguments The argument types.
	 * @param location The source location.
	 * @param message The message to log.
	 * @param arguments The arguments to be passed.
	 */
	template<class Message, class... Arguments>
	void TraceLog(std::source_location&& location, Message&& message, Arguments&&... arguments)
	{
		spdlog::info("[Trace @\"{}\":{}] {}", location.file_name(), location.line(), std::move(message), std::forward<Arguments>(arguments)...);
	}
}

#define XENON_LOG_LEVEL_ALL 5
#define XENON_LOG_LEVEL_NONE 0

#define XENON_LOG_LEVEL XENON_LOG_LEVEL_ALL

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
#						define XENON_LOG_TRACE(msg,...)			::Xenon::TraceLog(std::source_location::current(), msg, __VA_ARGS__)

#					endif
#				endif
#			endif
#		endif
#	endif
#endif // XENON_LOG_LEVEL

#ifndef XENON_LOG_FATAL
#	define XENON_LOG_FATAL(...)									::Xenon::NoOp()
#endif

#ifndef  XENON_LOG_ERROR
#	define XENON_LOG_ERROR(...)									::Xenon::NoOp()
#endif

#ifndef XENON_LOG_WARNING
#	define XENON_LOG_WARNING(...)								::Xenon::NoOp()
#endif

#ifndef XENON_LOG_INFORMATION
#	define XENON_LOG_INFORMATION(...)							::Xenon::NoOp()
#endif

#ifndef XENON_LOG_TRACE
#	define XENON_LOG_TRACE(...)							::Xenon::NoOp()
#endif

#ifdef XENON_DEBUG
#define XENON_LOG_DEBUG(...)									SPDLOG_DEBUG(__VA_ARGS__)

#else
#define XENON_LOG_DEBUG(...)									::Xenon::NoOp()

#endif // XENON_DEBUG

#define XENON_ASSERT(condition, ...)							if(!(condition)) XENON_LOG_FATAL(__VA_ARGS__)
