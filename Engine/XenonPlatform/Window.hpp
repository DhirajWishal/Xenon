// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonCore/Common.hpp"

#include <string>

namespace Xenon
{
	namespace Platform
	{
		/**
		 * Window class.
		 */
		class Window
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param title The title of the window.
			 * @param width The window's width.
			 * @param height The window's height.
			 */
			explicit Window(const std::string& title, uint32_t width, uint32_t height) : m_Title(title), m_Width(width), m_Height(height) {}

			/**
			 * Default virtual destructor.
			 */
			virtual ~Window() = default;

			/**
			 * Update the window.
			 * This will also poll the incoming window updates.
			 */
			virtual void update() = 0;

		protected:
			std::string m_Title;

			uint32_t m_Width = 0;
			uint32_t m_Height = 0;
		};
	}
}