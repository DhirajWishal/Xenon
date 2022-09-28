// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Interface factory class.
		 * This factory creates backend classes from a single backend.
		 */
		class IFactory
		{
		public:
			/**
			 * Default constructor.
			 */
			IFactory() = default;

			/**
			 * Default virtual constructor.
			 */
			virtual ~IFactory() = default;

			/**
			 * Create a new instance.
			 *
			 * @param appliationName The name of the application.
			 * @param applicationVersion The application version.
			 * @return The instance pointer.
			 */
			virtual std::unique_ptr<Instance> createInstance(const std::string& applicationName, uint32_t applicationVersion) = 0;
		};
	}
}