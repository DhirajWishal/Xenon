// Copyright 2022-2023 Dhiraj Wishal
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

		// Setup the default image, image view and image sampler.
		Backend::ImageSpecification imageSpecification = {};
		imageSpecification.m_Width = 1;
		imageSpecification.m_Height = 1;
		imageSpecification.m_Format = Xenon::Backend::DataFormat::R8G8B8A8_SRGB;
		m_pDefaultImage = m_pFactory->createImage(m_pDevice.get(), imageSpecification);

		m_pDefaultImageView = m_pFactory->createImageView(m_pDevice.get(), m_pDefaultImage.get(), {});
		m_pDefaultImageSampler = m_pFactory->createImageSampler(m_pDevice.get(), {});
	}

	Instance::~Instance()
	{
		m_MaterialDatabase.clear();

		m_pDefaultImage.reset();
		m_pDefaultImageView.reset();
		m_pDefaultImageSampler.reset();

		m_pDevice.reset();
		m_pInstance.reset();
	}

	void Instance::cleanup() const
	{
		m_pDevice->waitIdle();
	}
}
