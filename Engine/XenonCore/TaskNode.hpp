// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "JobSystem.hpp"

namespace Xenon
{
	/**
	 * Task node class.
	 * Task nodes are unique objects which encapsulates a single task in a task graph.
	 */
	class TaskNode final : public std::enable_shared_from_this<TaskNode>
	{
		friend class TaskGraph;

	public:
		/**
		 * Explicit constructor.
		 *
		 * @tparam Function The function type.
		 * @param jobSystem The job system.
		 * @param function The function to run.
		 * @param waitCount The number of parent tasks to wait on.
		 */
		template<class Function>
		explicit TaskNode(JobSystem& jobSystem, Function&& function, uint64_t waitCount)
			: m_JobSystem(jobSystem)
			, m_Task(std::forward<Function>(function))
			, m_WaitCount(waitCount)
		{
		}

		/**
		 * Start the execution.
		 * This will not do anything if it's waiting tasks have not been completed yet.
		 */
		void start();

		/**
		 * Create a child which will get executed after this.
		 *
		 * @tparam Function The function type.
		 * @param function The function to run.
		 * @return The child node.
		 */
		template<class Function>
		[[nodiscard]] std::shared_ptr<TaskNode> then(Function&& function)
		{
			auto pChild = std::make_shared<TaskNode>(m_JobSystem, std::forward<Function>(function), 1);
			addDependency(pChild);

			return pChild;
		}

		/**
		 * Reset the current task and set it's completion to false.
		 * It'll add this class as a dependency to the parents to be run after their completion.
		 *
		 * @tparam TaskNodes The parent task node types.
		 * @param pTasks The parent task pointers.
		 */
		template<class... TaskNodes>
		void reset(const std::shared_ptr<TaskNodes>&... pTasks)
		{
			m_WaitCount = sizeof...(pTasks);
			m_Completed = false;

			if constexpr (sizeof...(pTasks) > 0)
			{
				auto pThis = shared_from_this();
				(pTasks->addDependency(pThis), ...);
			}
		}

		/**
		 * Reset the current task and set it's completion to false.
		 * It'll add this class as a dependency to the parents to be run after their completion.
		 *
		 * @param pTasks The parent task pointers.
		 */
		void reset(const std::vector<std::shared_ptr<TaskNode>>& pTasks);

		/**
		 * Check if the node has completed it's execution.
		 *
		 * @return True if the task has been executed.
		 * @return False if the task has not been executed.
		 */
		[[nodiscard]] bool isComplete() const { return m_Completed; }

		/**
		 * Wait till the node has been executed.
		 * Make sure to call start before calling this method!
		 */
		void wait() const;

	private:
		/**
		 * Insert this class to the job system's queue.
		 */
		void insertThis();

		/**
		 * Add a dependency to the node.
		 * If the node is complete, it'll not add the child.
		 *
		 * @param pNode The child node.
		 */
		void addDependency(const std::shared_ptr<TaskNode>& pNode);

		/**
		 * This function gets invoked by the parent class when it's completed.
		 * It will decrement the internal wait counter and if the count is 0 (we don't need to wait anymore)
		 * it'll push the internal task to the job system.
		 *
		 * @param forceRun If this variable is set to true, it'll attempt to run the task on the calling thread.
		 */
		void onParentCompletion(bool forceRun);

		/**
		 * Signal all the children that we have completed execution.
		 * If there is only one child, it will try to directly run the child in this thread.
		 */
		void signalChildren();

		/**
		 * Run the current task and signal all the child nodes.
		 */
		void run();

	private:
		JobSystem& m_JobSystem;

		std::vector<std::shared_ptr<TaskNode>> m_pChildren;

		std::mutex m_Mutex;

		std::function<void()> m_Task;
		std::atomic<uint64_t> m_WaitCount = 0;

		std::atomic_bool m_Completed = false;
	};
}