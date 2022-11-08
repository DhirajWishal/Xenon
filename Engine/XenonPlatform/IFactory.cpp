// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "IFactory.hpp"

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
#error "The platform is not supported!"

#endif
		}

		std::unique_ptr<Xenon::Platform::IFactory> IFactory::Create()
		{
			return nullptr;

#if defined(XENON_PLATFORM_WINDOWS)

#elif defiend(XENON_PLATFORM_LINUX)

#elif defiend(XENON_PLATFORM_MAC)

#else
#error "The platform is not supported!"

#endif
		}
	}
}