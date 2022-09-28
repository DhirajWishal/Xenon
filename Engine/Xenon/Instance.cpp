// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Instance.hpp"

#include "../XenonVulkanBackend/VulkanFactory.hpp"

namespace Xenon
{
	Instance::Instance(const std::string& applicationName, uint32_t applicationVersion, RenderTargetType renderTargets)
		: m_ApplicationName(applicationName)
		, m_ApplicationVersion(applicationVersion)
		, m_RenderTargets(renderTargets)
	{
		// If the global backend factory is not initialized, let's initialize it.
		if (Globals::BackendFactory == nullptr)
			Globals::BackendFactory = std::make_unique<Backend::VulkanFactory>();
	}
}