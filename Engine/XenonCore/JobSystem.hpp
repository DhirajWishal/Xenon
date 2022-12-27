// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <thread>
#include <functional>
#include <condition_variable>
#include <list>
#include <mutex>
#include <future>
#include <cstdint>

namespace Xenon
{
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
		 * Note that a job might start right after inserting it.
		 *
		 * @tparam Job The job type.
		 * @param job The job to insert.
		 * @return The job's return future.
		 */
		template<class Job>
		decltype(auto) insert(Job&& job)
		{
			using ReturnType = std::invoke_result_t<Job>;

			// Create the promise and get the future.
			auto pPromise = new std::promise<ReturnType>();
			auto future = pPromise->get_future();

			// Setup the job function.
			const auto jobFunction = [pPromise, job = std::move(job)]
			{
				try
				{
					if constexpr (std::is_void_v<ReturnType>)
					{
						job();
						pPromise->set_value();
					}
					else
					{
						pPromise->set_value(job());
					}

					delete pPromise;
				}
				catch (std::exception_ptr ptr)
				{
					pPromise->set_exception(ptr);
				}
			};

			// Insert the job to the system.
			const auto lock = std::scoped_lock(m_JobMutex);
			const auto jobStatus = m_JobEntries.emplace_back(std::move(jobFunction));

			m_ConditionVariable.notify_one();
			return future;
		}

		/**
		 * Update the thread count.
		 * Note that this might block the calling thread.
		 *
		 * @param theradCount The number of threads the job system will have.
		 */
		void setThreadCount(uint32_t threadCount);

		/**
		 * Wait till all the submitted jobs are completed.
		 */
		void wait();

		/**
		 * Wait for a timeout nanoseconds till the submitted jobs are completed.
		 *
		 * @param timeout The timeout time to wait in nanoseconds.
		 */
		void waitFor(std::chrono::nanoseconds timeout);

		/**
		 * Clear the job system.
		 * This will close all the workers and complete everything that has been issued.
		 */
		void clear();

		/**
		 * Check if all the submitted work has been completed.
		 *
		 * @return True if the jobs have been completed.
		 * @return False if the jobs have not been completed.
		 */
		[[nodiscard]] bool isComplete();

		/**
		 * Get the number of threads used by the system.
		 *
		 * @return The thread count.
		 */
		[[nodiscard]] uint64_t getThreadCount() const noexcept { return m_Workers.size(); }

	private:
		/**
		 * This function is the worker function which is run on a separate thread.
		 *
		 * @param index The thread index.
		 */
		void worker(uint32_t index);

		/**
		 * This function will execute a single job in the worker thread.
		 *
		 * @param lock The lock to lock and unlock when executing.
		 * @param index The thread index.
		 */
		void execute(std::unique_lock<std::mutex>& lock, uint32_t index);

	private:
		std::mutex m_JobMutex;
		std::atomic_bool m_ShouldRun = true;
		std::atomic_bool m_ShouldFinishJobs = true;
		std::condition_variable m_ConditionVariable;

		std::list<std::function<void()>> m_JobEntries;
		std::vector<std::jthread> m_Workers;
		std::vector<bool> m_WorkerState;
	};
}