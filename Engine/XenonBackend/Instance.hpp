// Copyright 2022-2023 Dhiraj Wishal
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
			explicit Instance([[maybe_unused]] const std::string& applicationName, [[maybe_unused]] uint32_t applicationVersion) {}
		};
	}
}