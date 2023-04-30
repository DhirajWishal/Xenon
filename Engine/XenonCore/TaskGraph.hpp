// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "TaskNode.hpp"

namespace Xenon
{
	/**
	 * Task graph class.
	 * This class contains all the logic to manage an active task graph.
	 */
	class TaskGraph final
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param jobSystem The job system to use.
		 */
		explicit TaskGraph(JobSystem& jobSystem) : m_JobSystem(jobSystem) {}

		/**
		 * Create a new task node.
		 *
		 * @tparam Function The function type.
		 * @tparam TaskNodes The parent task nodes.
		 * @param jobSystem The job system to insert the task to.
		 * @param function The function to run as the task.
		 * @param pTasks The parent tasks.
		 * @return The task pointer.
		 */
		template<class Function, class... TaskNodes>
		[[nodiscard]] std::shared_ptr<TaskNode> create(Function&& function, const std::shared_ptr<TaskNodes>&... pTasks)
		{
			constexpr auto parentCount = sizeof...(pTasks);
			auto pChild = std::make_shared<TaskNode>(m_JobSystem, std::forward<Function>(function), parentCount);

			if constexpr (parentCount > 0)
				(pTasks->addDependency(pChild), ...);

			return pChild;
		}

		/**
		 * Create a new task node.
		 *
		 * @tparam Function The function type.
		 * @param jobSystem The job system to insert the task to.
		 * @param function The function to run as the task.
		 * @param pTasks The parent tasks.
		 * @return The task pointer.
		 */
		template<class Function>
		[[nodiscard]] std::shared_ptr<TaskNode> create(Function&& function, const std::vector<std::shared_ptr<TaskNode>>& pTasks)
		{
			auto pChild = std::make_shared<TaskNode>(m_JobSystem, std::forward<Function>(function), pTasks.size());
			for (const auto& pTask : pTasks)
				pTask->addDependency(pChild);

			return pChild;
		}

		/**
		 * Wait till all the tasks have been completed.
		 */
		void complete() { m_JobSystem.wait(); }

	private:
		JobSystem& m_JobSystem;
	};
}