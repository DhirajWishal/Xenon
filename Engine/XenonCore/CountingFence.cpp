// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "CountingFence.hpp"

#include <optick.h>

namespace Xenon
{
	void CountingFence::arrive(uint64_t decrement /*= 1*/)
	{
		OPTICK_EVENT();

		m_Counter -= decrement;
		// if (m_Counter == 0) m_Counter.notify_all();
	}

	void CountingFence::wait() const
	{
		OPTICK_EVENT();

		// TODO: Implement a blocking approach. Maybe using m_Counter.wait(0);
		while (m_Counter > 0);
		// m_Counter.wait(0);
	}

	void CountingFence::reset(uint64_t value)
	{
		OPTICK_EVENT();

		m_Counter = value;
	}
}