// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Instance.hpp"

#include "../XenonVulkanBackend/VulkanFactory.hpp"

#ifdef XENON_PLATFORM_WINDOWS
#include "../XenonDX12Backend/DX12Factory.hpp"

#endif // XENON_PLATFORM_WINDOWS

namespace Xenon
{
	Instance::Instance(const std::string& applicationName, uint32_t applicationVersion, RenderTargetType renderTargets, BackendType backendType /*= BackendType::Any*/)
		: m_ApplicationName(applicationName)
		, m_ApplicationVersion(applicationVersion)
	{
		// If the global backend factory is not initialized, let's initialize it.
		if (Globals::BackendFactory == nullptr)
		{
#ifdef XENON_PLATFORM_WINDOWS
			if (backendType == BackendType::Any || backendType == BackendType::Vulkan)
				Globals::BackendFactory = std::make_unique<Backend::VulkanFactory>();
			else
				Globals::BackendFactory = std::make_unique<Backend::DX12Factory>();

#else 
			Globals::BackendFactory = std::make_unique<Backend::VulkanFactory>();

			if (backendType == BackendType::DirectX_12)
				XENON_LOG_WARNING("DirectX 12 is not supported in the current platform. The current backend is set to Vulkan.");

#endif // XENON_PLATFORM_WINDOWS
		}

		// Create the instance.
		m_pInstance = Globals::BackendFactory->createInstance(applicationName, applicationVersion);

		// Create the device.
		m_pDevice = Globals::BackendFactory->createDevice(m_pInstance.get(), renderTargets);
	}

	Instance::~Instance()
	{
		m_pDevice.reset();
		m_pInstance.reset();
	}
}
