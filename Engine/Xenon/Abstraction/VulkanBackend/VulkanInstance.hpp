// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../Instance.hpp"

#include <volk.h>

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Instance class.
		 * This is the base class for the backend instance.
		 */
		class VulkanInstance final : public Instance
		{
		public:
			/**
			 * Constructor.
			 */
			VulkanInstance();
		};
	}
}