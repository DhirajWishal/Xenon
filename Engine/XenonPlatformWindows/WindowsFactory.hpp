// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonPlatform/IFactory.hpp"

namespace Xenon
{
	namespace Platform
	{
		/**
		 * Windows factory class.
		 */
		class WindowsFactory final : public IFactory
		{
		public:
			/**
			 * Default constructor.
			 */
			WindowsFactory() = default;

			/**
			 * Default destructor.
			 */
			~WindowsFactory() override = default;

			/**
			 * Create a new window.
			 *
			 * @param title The title of the window.
			 * @param width The width of the window.
			 * @param height The height of the window.
			 * @return The window pointer.
			 */
			[[nodiscard]] std::unique_ptr<Window> createWindow(const std::string& title, uint32_t width, uint32_t height) override;
		};
	}
}