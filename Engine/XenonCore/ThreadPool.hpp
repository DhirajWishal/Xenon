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
	 * Thread pool class.
	 * A thread pool contains multiple threads to which a user defined function can be passed to perform any task.
	 */
	class ThreadPool final
	{
	public:
	private:
		std::vector<std::jthread> m_Workers;
	};
}