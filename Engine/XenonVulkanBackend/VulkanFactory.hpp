// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/IFactory.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Vulkan factory class.
		 * This is used to create Vulkan backend objects and is used by the abstraction layer and the frontend.
		 */
		class VulkanFactory final : public IFactory
		{
		public:
			/**
			 * Default constructor.
			 */
			VulkanFactory() = default;

			/**
			 * Default destructor.
			 */
			~VulkanFactory() override = default;

			/**
			 * Create a new instance.
			 *
			 * @return The instance pointer.
			 */
			std::unique_ptr<Instance> createInstance() override;
		};
	}
}