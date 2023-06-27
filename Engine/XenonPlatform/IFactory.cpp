// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "IFactory.hpp"

// TODO: This should me moved out from this project to the backend or engine.
#if defined(XENON_PLATFORM_WINDOWS)
#include "../XenonPlatformWindows/WindowsFactory.hpp"

#elif defined(XENON_PLATFORM_LINUX)
#include "../XenonPlatformLinux/LinuxFactory.hpp"

#elif defined(XENON_PLATFORM_MAC)

#else
#error "The platform is not supported!"

#endif // defined(XENON_PLATFORM_WINDOWS)

namespace Xenon
{
	namespace Platform
	{
		Xenon::Platform::SupportedPlatforms GetCurrentPlatform()
		{
#if defined(XENON_PLATFORM_WINDOWS)
			return SupportedPlatforms::Windows;

#elif defined(XENON_PLATFORM_LINUX)
			return SupportedPlatforms::Linux;

#elif defined(XENON_PLATFORM_MAC)
			return SupportedPlatforms::MacOS;

#else
			return SupportedPlatforms::Windows;

#endif // defined(XENON_PLATFORM_WINDOWS)
		}

		Xenon::Platform::IFactory* IFactory::Get()
		{
#if defined(XENON_PLATFORM_WINDOWS)
			static Xenon::Platform::WindowsFactory factory;
			return &factory;

#elif defined(XENON_PLATFORM_LINUX)
			static Xenon::Platform::LinuxFactory factory;
			return &factory;

#elif defined(XENON_PLATFORM_MAC)

#else
			return nullptr;

#endif // defined(XENON_PLATFORM_WINDOWS)
		}
	}
}