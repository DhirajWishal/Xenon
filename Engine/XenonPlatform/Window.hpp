// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonCore/Common.hpp"
#include "../XenonCore/XObject.hpp"

#include <string>

namespace Xenon
{
	namespace Platform
	{
		/**
		 * Window feature enum.
		 * This defines what features the window should have.
		 */
		enum class WindowFeature : uint32_t
		{
			None = 0,

			AcceptDragAndDrop = XENON_BIT_SHIFT(0),
			EnableBorders = XENON_BIT_SHIFT(1),
		};

		XENON_DEFINE_ENUM_OR(WindowFeature);
		XENON_DEFINE_ENUM_AND(WindowFeature);

		/**
		 * Window class.
		 */
		class Window : public XObject
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

			/**
			 * Check if the window is still open.
			 *
			 * @return True if the window is open.
			 * @return False if the window is closed.
			 */
			[[nodiscard]] virtual bool isOpen() const = 0;

			/**
			 * Get the width of the window.
			 *
			 * @return The window's width.
			 */
			[[nodiscard]] uint32_t getWidth() const { return m_Width; }

			/**
			 * Get the height of the window.
			 *
			 * @return The window's height.
			 */
			[[nodiscard]] uint32_t getHeight() const { return m_Height; }

		protected:
			std::string m_Title;

			uint32_t m_Width = 0;
			uint32_t m_Height = 0;
		};
	}
}