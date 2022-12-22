// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "CountingFence.hpp"

namespace Xenon
{
	void CountingFence::arrive(uint64_t decrement /*= 1*/)
	{
		m_Counter -= decrement;
	}

	void CountingFence::wait() const
	{
		// TODO: Implement a blocking approach. Maybe using m_Counter.wait(0);
		while (m_Counter > 0);
	}

	void CountingFence::reset(uint64_t value)
	{
		m_Counter = value;
	}
}