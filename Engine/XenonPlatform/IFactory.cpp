// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "IFactory.hpp"

#if defined(XENON_PLATFORM_WINDOWS)
#include "../XenonPlatformWindows/WindowsFactory.hpp"

#elif defiend(XENON_PLATFORM_LINUX)

#elif defiend(XENON_PLATFORM_MAC)

#else
#error "The platform is not supported!"

#endif

namespace Xenon
{
	namespace Platform
	{
		Xenon::Platform::SupportedPlatforms GetCurrentPlatform()
		{
#if defined(XENON_PLATFORM_WINDOWS)
			return SupportedPlatforms::Windows;

#elif defiend(XENON_PLATFORM_LINUX)
			return SupportedPlatforms::Linux;

#elif defiend(XENON_PLATFORM_MAC)
			return SupportedPlatforms::MacOS;

#else
			return SupportedPlatforms::Windows;

#endif
		}

		std::unique_ptr<Xenon::Platform::IFactory> IFactory::Create()
		{
#if defined(XENON_PLATFORM_WINDOWS)
			return std::make_unique<Xenon::Platform::WindowsFactory>();

#elif defiend(XENON_PLATFORM_LINUX)

#elif defiend(XENON_PLATFORM_MAC)

#else
			return nullptr;

#endif
		}
	}
}