// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "System.hpp"

#include <thread>

namespace Xenon
{
	/**
	 * System handler class.
	 * This class holds information about a single system and is used to issue requests and handle the system thread.
	 *
	 * Note that this class is a singleton.
	 *
	 * @tparam Type The system type.
	 */
	template<class Type>
	class SystemHandler final
	{
		static_assert(!std::is_base_of_v<Type, System>, "Invalid system type! Make sure that the 'Type' is derived from the 'System' class.");

		/**
		 * Private default constructor.
		 */
		SystemHandler()
		{
			m_Worker = std::jthread([this]
				{
					// Signal that we're starting the thread.
					m_System.onStart();

					do
					{
						// Call the on update method as the first thing.
						m_System.onUpdate();

						// Handle the requests.
						m_System.handleRequests();
					} while (m_bShouldRun);

					// Handle any more requests if they're present.
					m_System.handleRequests();

					// Call the on termination method to signal that we're ending the system.
					m_System.onTermination();
				}
			);
		}

		/**
		 * Private default destructor.
		 */
		~SystemHandler()
		{
			terminate();
		}

	public:
		XENON_DISABLE_COPY(SystemHandler);
		XENON_DISABLE_MOVE(SystemHandler);

		/**
		 * Get the system instance.
		 *
		 * @return The handler reference.
		 */
		static SystemHandler<Type>& Instance()
		{
			static SystemHandler<Type> instance;
			return instance;
		}

		/**
		 * This method will terminate the system.
		 * Note that this method will wait till the system thread joins back.
		 */
		void terminate()
		{
			if (m_bShouldRun)
			{
				m_bShouldRun = false;
				m_Worker.join();
			}
		}

		/**
		 * Issue a request to the system.
		 *
		 * @tparam Request The request type.
		 * @tparam Arguments The request constructor argument types.
		 * @param arguments The arguments to be passed to the constructor.
		 * @return The created request pointer.
		 */
		template<class Request, class... Arguments>
		Request* issueRequest(Arguments&&... arguments)
		{
			return m_System.issueRequest<Request>(std::forward<Arguments>(arguments)...);
		}

	private:
		Type m_System;

		std::jthread m_Worker;
		std::atomic_bool m_bShouldRun = true;
	};
}