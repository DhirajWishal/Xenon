// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "JobSystem.hpp"
#include "Logging.hpp"

#include <optick.h>

#include <latch>

namespace Xenon
{
	JobSystem::JobSystem(uint32_t threadCount)
		: m_WorkerState(threadCount, false)
	{
		auto latch = std::latch(threadCount);

		m_Workers.reserve(threadCount);
		for (uint32_t i = 0; i < threadCount; i++)
			m_Workers.emplace_back([this, &latch, i] { latch.count_down(); worker(i); });

		// Wait till all the workers have started.
		latch.wait();
	}

	JobSystem::~JobSystem()
	{
		clear();
	}

	void JobSystem::setThreadCount(uint32_t threadCount)
	{
		// Close the previous workers.
		m_ShouldFinishJobs = false;
		clear();

		// Create and start the new ones.
		m_ShouldRun = true;
		m_ShouldFinishJobs = true;

		m_WorkerState.resize(threadCount, false);
		m_Workers.reserve(threadCount);

		for (uint32_t i = 0; i < threadCount; i++)
			m_Workers.emplace_back([this, i] { worker(i); });
	}

	void JobSystem::wait()
	{
		OPTICK_EVENT();

		while (!isComplete());
	}

	void JobSystem::waitFor(std::chrono::nanoseconds timeout)
	{
		OPTICK_EVENT();

		const auto targetTimeStamp = std::chrono::high_resolution_clock::now() + timeout;
		while (!isComplete() && targetTimeStamp > std::chrono::high_resolution_clock::now());
	}

	void JobSystem::clear()
	{
		OPTICK_EVENT();

		m_ShouldRun = false;
		m_ConditionVariable.notify_all();
		m_Workers.clear();
		m_WorkerState.clear();
	}

	bool JobSystem::isComplete()
	{
		const auto lock = std::scoped_lock(m_JobMutex);

		// If the entries are not empty, return false.
		if (!m_JobEntries.empty())
			return false;

		// Else if at least one state is running, return false.
		for (const auto state : m_WorkerState)
		{
			if (state)
				return false;
		}

		// Else we're truly complete.
		return true;
	}

	void JobSystem::worker(uint32_t index)
	{
		const auto threadTitle = fmt::format("Worker thread ({}) number ({})", fmt::ptr(this), index);
		OPTICK_THREAD(threadTitle.c_str());

		auto locker = std::unique_lock(m_JobMutex);

		do
		{
			// Wait till we get notified, or if the job entries have jobs to execute, or if we can end the thread.
			m_ConditionVariable.wait(locker, [this] { return !m_JobEntries.empty() || m_ShouldRun == false; });

			// If we have jobs, execute one.
			if (!m_JobEntries.empty())
				execute(locker, index);
		} while (m_ShouldRun);


		// Finish up the remaining jobs if needed and there are some left.
		if (m_ShouldFinishJobs)
		{
			while (!m_JobEntries.empty())
				execute(locker, index);
		}
	}

	void JobSystem::execute(std::unique_lock<std::mutex>& lock, uint32_t index)
	{
		OPTICK_EVENT();

		m_WorkerState[index] = true;

		// Get the next job entry and pop it from the list.
		const auto jobEntry = m_JobEntries.front();
		m_JobEntries.pop_front();

		// Unlock the locker.
		if (lock) lock.unlock();

		// Execute the job.
		{
			OPTICK_EVENT_DYNAMIC("Executing Job");

			jobEntry();
			m_WorkerState[index] = false;
		}

		// Lock it back again.
		OPTICK_EVENT_DYNAMIC("Locking Mutex");
		lock.lock();
	}
}