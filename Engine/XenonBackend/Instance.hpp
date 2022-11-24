// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "BackendObject.hpp"

#include <string>
#include <memory>

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Instance class.
		 * This is the base class for the backend instance.
		 */
		class Instance : public BackendObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param appliationName The name of the application.
			 * @param applicationVersion The application version.
			 */
			explicit Instance([[maybe_unused]] const std::string& applicationName, [[maybe_unused]] uint32_t applicationVersion) : m_DeletionQueue(1) {}

		public:
			/**
			 * Get the deletion queue.
			 * The deletion queue is used to asynchronously destroy object's backend handles without affecting the application runtime.
			 *
			 * @return The job system reference.
			 */
			[[nodiscard]] JobSystem& getDeletionQueue() { return m_DeletionQueue; }

			/**
			 * Get the deletion queue.
			 * The deletion queue is used to asynchronously destroy object's backend handles without affecting the application runtime.
			 *
			 * @return The job system const reference.
			 */
			[[nodiscard]] const JobSystem& getDeletionQueue() const { return m_DeletionQueue; }

		private:
			JobSystem m_DeletionQueue;
		};
	}
}