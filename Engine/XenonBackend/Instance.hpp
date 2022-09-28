// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Instance class.
		 * This is the base class for the backend instance.
		 */
		class Instance
		{
		public:
			/**
			 * Default constructor.
			 */
			Instance() = default;

			/**
			 * Default virtual destructor.
			 */
			virtual ~Instance() = default;
		};
	}
}