// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "System.hpp"

namespace Xenon
{
	System::System()
	{
		m_Worker = std::jthread([this]
			{
				// Signal that we're starting the thread.
				onStart();

				while (m_bShouldRun)
				{
					// Call the on update method as the first thing.
					onUpdate();

					// Handle the requests.
					handleRequests();
				}

				// Handle any more requests if they're present.
				handleRequests();

				// Call the on termination method to signal that we're ending the system.
				onTermination();
			}
		);
	}

	System::~System()
	{
		m_bShouldRun = false;
	}

	void System::terminate()
	{
		m_bShouldRun = false;
	}

	void System::handleRequests()
	{
		const auto lock = std::scoped_lock(m_RequestMutex);
		for (auto& request : m_Requests)
			request();

		m_Requests.clear();
	}
}
