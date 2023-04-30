// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#include "CountingFence.hpp"

#include <optick.h>

namespace Xenon
{
	void CountingFence::arrive(uint64_t decrement /*= 1*/)
	{
		OPTICK_EVENT();

		m_Counter -= decrement;
		if (m_Counter == 0)
			m_ConditionVariable.notify_all();
	}

	void CountingFence::waitBlocking()
	{
		OPTICK_EVENT();

		auto lock = std::unique_lock(m_Mutex);
		m_ConditionVariable.wait(lock, [this] { return m_Counter == 0; });
	}

	void CountingFence::waitSpinning() const
	{
		OPTICK_EVENT();

		while (m_Counter > 0);
	}

	void CountingFence::wait()
	{
		OPTICK_EVENT();

		waitBlocking();
	}

	void CountingFence::wait() const
	{
		OPTICK_EVENT();

		waitSpinning();
	}

	void CountingFence::reset(uint64_t value)
	{
		OPTICK_EVENT();

		m_Counter = value;
	}
}