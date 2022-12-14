// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Instance.hpp"

#include "Materials/DefaultMaterial.hpp"

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
#ifdef XENON_PLATFORM_WINDOWS
		if (backendType == BackendType::Any || backendType == BackendType::Vulkan)
		{
			m_pFactory = std::make_unique<Backend::VulkanFactory>();
			m_BackendType = BackendType::Vulkan;
		}
		else
		{
			m_pFactory = std::make_unique<Backend::DX12Factory>();
			m_BackendType = BackendType::DirectX_12;
		}

#else 
		m_pFactory = std::make_unique<Backend::VulkanFactory>();
		m_BackendType = BackendType::Vulkan;

		if (backendType == BackendType::DirectX_12)
			XENON_LOG_WARNING("DirectX 12 is not supported in the current platform. The current backend is set to Vulkan.");

#endif // XENON_PLATFORM_WINDOWS

		// Create the instance.
		m_pInstance = m_pFactory->createInstance(applicationName, applicationVersion);

		// Create the device.
		m_pDevice = m_pFactory->createDevice(m_pInstance.get(), renderTargets);

		// Setup the default material.
		m_DefaultMaterialIdentifier = m_MaterialDatabase.create<DefaultMaterial>(0, *this);
	}

	Instance::~Instance()
	{
		m_MaterialDatabase.clear();
		m_pDevice.reset();
		m_pInstance.reset();
	}

	void Instance::cleanup() const
	{
		m_pDevice->waitIdle();
	}
}
