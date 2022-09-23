// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "JobSystem.hpp"

namespace Xenon
{
	JobSystem::JobSystem(uint32_t threadCount)
	{
		m_Workers.reserve(threadCount);
		for (uint32_t i = 0; i < threadCount; i++)
			m_Workers.emplace_back([this] { worker(); });
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

		m_Workers.reserve(threadCount);
		for (uint32_t i = 0; i < threadCount; i++)
			m_Workers.emplace_back([this] { worker(); });
	}

	void JobSystem::clear()
	{
		m_ShouldRun = false;
		m_ConditionVariable.notify_all();
		m_Workers.clear();
	}

	bool JobSystem::completed()
	{
		const auto lock = std::scoped_lock(m_JobMutex);
		return m_JobEntries.empty();
	}

	void JobSystem::worker()
	{
		auto locker = std::unique_lock(m_JobMutex);

		do
		{
			// Wait till we get notified, or if the job entries have jobs to execute, or if we can end the thread.
			m_ConditionVariable.wait(locker, [this] { return !m_JobEntries.empty() || m_ShouldRun == false; });

			// If we have jobs, execute one.
			if (!m_JobEntries.empty())
				execute(locker);

		} while (m_ShouldRun);


		// Finish up the remaining jobs if needed and there are some left.
		if (m_ShouldFinishJobs)
		{
			while (!m_JobEntries.empty())
				execute(locker);
		}
	}

	void JobSystem::execute(std::unique_lock<std::mutex>& lock)
	{
		// Get the next job entry and pop it from the list.
		const auto jobEntry = m_JobEntries.front();
		m_JobEntries.pop_front();

		// Unlock the locker.
		if (lock) lock.unlock();

		// Notify that we're executing.
		jobEntry.m_pJobStatus->store(JobStatus::Executing);

		// Execute the job.
		jobEntry.m_Job();

		// Notify that we've completed the job.
		jobEntry.m_pJobStatus->store(JobStatus::Completed);

		// Lock it back again.
		lock.lock();
	}
}