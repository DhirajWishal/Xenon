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
			explicit Instance(const std::string& applicationName, uint32_t applicationVersion) {}

			/**
			 * Default virtual destructor.
			 */
			virtual ~Instance() = default;
		};
	}
}