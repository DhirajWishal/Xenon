// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanFactory.hpp"

#include "VulkanInstance.hpp"

namespace Xenon
{
	namespace Backend
	{
		std::unique_ptr<Xenon::Backend::Instance> VulkanFactory::createInstance()
		{
			return std::make_unique<VulkanInstance>();
		}
	}
}