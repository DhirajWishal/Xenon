// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "FrameTimer.hpp"

namespace Xenon
{
	std::chrono::nanoseconds FrameTimer::tick()
	{
		const auto current = Clock::now();
		const auto diff = current - m_OldTime;
		m_OldTime = current;

		return diff;
	}
}