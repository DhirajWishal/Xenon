// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <thread>
#include <atomic>
#include <functional>
#include <condition_variable>
#include <list>
#include <mutex>

namespace Xenon
{
	/**
	 * Job status enum.
	 */
	enum class JobStatus : uint8_t
	{
		Pending,
		Executing,
		Completed
	};

	/**
	 * Job entry structure.
	 * This contains the actual job and it's current state.
	 */
	struct JobEntry final
	{
		/**
		 * Explicit constructor.
		 *
		 * @tparam Job The job type.
		 * @param job The job to execute.
		 */
		template<class Job>
		explicit JobEntry(Job&& job) : m_Job(std::move(job)), m_pJobStatus(std::make_shared<std::atomic<JobStatus>>(JobStatus::Pending)) {}

		std::function<void()> m_Job;
		std::shared_ptr<std::atomic<JobStatus>> m_pJobStatus;
	};

	/**
	 * Job system class.
	 * This contains multiple threads, which simultaneously executes a job which is been given to the system.
	 */
	class JobSystem final
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param theradCount The number of worker threads needed.
		 */
		explicit JobSystem(uint32_t threadCount);

		/**
		 * Destructor.
		 */
		~JobSystem();

		/**
		 * Insert a new job to the job system.
		 *
		 * @tparam Job The job type.
		 * @param job The job to insert.
		 * @return The job status pointer.
		 */
		template<class Job>
		std::shared_ptr<const std::atomic<JobStatus>> insert(Job&& job)
		{
			const auto lock = std::scoped_lock(m_JobMutex);
			const auto jobStatus = m_JobEntries.emplace_back(JobEntry(std::move(job))).m_pJobStatus;
			m_ConditionVariable.notify_one();

			return jobStatus;
		}

		/**
		 * Update the thread count.
		 * Note that this might block the calling thread.
		 *
		 * @param theradCount The number of threads the job system will have.
		 */
		void setThreadCount(uint32_t threadCount);

		/**
		 * Clear the job system.
		 * This will close all the workers and complete everything that has been issued.
		 */
		void clear();

		/**
		 * Check if all the submitted work has been completed.
		 *
		 * @return Whether the jobs have been completed.
		 */
		[[nodiscard]] bool completed();

		/**
		 * Get the number of threads used by the system.
		 *
		 * @return The thread count.
		 */
		[[nodiscard]] uint64_t getThreadCount() const { return m_Workers.size(); }

	private:
		/**
		 * This function is the worker function which is run on a separate thread.
		 */
		void worker();

		/**
		 * This function will execute a single job in the worker thread.
		 *
		 * @param lock The lock to lock and unlock when executing.
		 */
		void execute(std::unique_lock<std::mutex>& lock);

	private:
		std::mutex m_JobMutex;
		std::atomic_bool m_ShouldRun = true;
		std::atomic_bool m_ShouldFinishJobs = true;
		std::condition_variable m_ConditionVariable;

		std::list<JobEntry> m_JobEntries;
		std::vector<std::jthread> m_Workers;
	};
}