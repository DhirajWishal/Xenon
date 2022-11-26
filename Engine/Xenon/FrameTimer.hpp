// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>

namespace Xenon
{
	/**
	 * Frame timer class.
	 * This utility class contains an internal clock and can be used to find the time taken by the previous frame to the current frame.
	 */
	class FrameTimer final
	{
		using Clock = std::chrono::system_clock;
		using TimePoint = Clock::time_point;

	public:
		/**
		 * Default constructor.
		 */
		FrameTimer() = default;

		/**
		 * Update the internal time and get the time taken from the previous tick (or creation) to now.
		 *
		 * @return The time taken in nanoseconds.
		 */
		[[nodiscard]] std::chrono::nanoseconds tick();

	private:
		TimePoint m_OldTime = Clock::now();
	};
}