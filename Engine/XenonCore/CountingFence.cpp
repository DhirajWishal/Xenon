// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "CountingFence.hpp"

#include <optick.h>

namespace Xenon
{
	void CountingFence::arrive(uint64_t decrement /*= 1*/)
	{
		OPTICK_EVENT();

		m_Counter -= decrement;
	}

	void CountingFence::wait() const
	{
		OPTICK_EVENT();

		while (m_Counter > 0);
	}

	void CountingFence::reset(uint64_t value)
	{
		OPTICK_EVENT();

		m_Counter = value;
	}
}