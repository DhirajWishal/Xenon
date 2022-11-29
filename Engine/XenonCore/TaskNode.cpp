// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "TaskNode.hpp"

namespace Xenon
{
	void TaskNode::reset(const std::vector<std::shared_ptr<TaskNode>>& pTasks)
	{
		m_Completed = false;
		m_WaitCount = pTasks.size();

		auto pThis = shared_from_this();
		for (const auto& pTask : pTasks)
			pTask->addDependency(pThis);
	}

	void TaskNode::insertThis()
	{
		m_JobSystem.insert([pThis = shared_from_this()] { pThis->run(); });
	}

	void TaskNode::addDependency(const std::shared_ptr<TaskNode>& pNode)
	{
		if (m_Completed)
			pNode->onParentCompletion(false);

		else
		{
			auto lock = std::scoped_lock(m_Mutex);
			m_pChildren.emplace_back(pNode);
		}
	}

	void TaskNode::onParentCompletion(bool forceRun)
	{
		if (--m_WaitCount == 0)
		{
			if (forceRun)
				run();

			else
				insertThis();
		}
	}

	void TaskNode::signalChildren()
	{
		if (m_pChildren.size() == 1)
		{
			m_pChildren.front()->onParentCompletion(true);
		}
		else
		{
			for (const auto& pChild : m_pChildren)
				pChild->onParentCompletion(false);
		}

		m_pChildren.clear();
	}

	void TaskNode::run()
	{
		m_Task();
		signalChildren();

		m_Completed = true;
	}
}