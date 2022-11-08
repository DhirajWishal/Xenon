// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Window.hpp"

#include <memory>

namespace Xenon
{
	namespace Platform
	{
		/**
		 * Supported platforms enum.
		 */
		enum class SupportedPlatforms : uint8_t
		{
			Windows,
			Linux,	// Not there yet.
			MacOS	// Not there yet.
		};

		/**
		 * Get the current platform.
		 *
		 * @return The platform.
		 */
		[[nodiscard]] SupportedPlatforms GetCurrentPlatform();

		/**
		 * Interface factory class.
		 * This class is used to create platform abstracted objects.
		 */
		class IFactory
		{
		public:
			/**
			 * Default constructor.
			 */
			IFactory() = default;

			/**
			 * Default virtual destructor.
			 */
			virtual ~IFactory() = default;

			/**
			 * Create a new platform-specific factory.
			 *
			 * @return The factory object pointer.
			 */
			[[nodiscard]] static std::unique_ptr<IFactory> Create();
		};
	}
}