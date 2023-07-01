// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonCore/Common.hpp"
#include "../XenonCore/XObject.hpp"

#include "../XenonEvents/Mouse.hpp"
#include "../XenonEvents/Keyboard.hpp"
#include "../XenonEvents/GeneralEvents.hpp"

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
			XENON_NODISCARD virtual bool isOpen() const = 0;

			/**
			 * Get the width of the window.
			 *
			 * @return The window's width.
			 */
			XENON_NODISCARD uint32_t getWidth() const { return m_Width; }

			/**
			 * Get the height of the window.
			 *
			 * @return The window's height.
			 */
			XENON_NODISCARD uint32_t getHeight() const { return m_Height; }

			/**
			 * Get the mouse inputs.
			 *
			 * @return The mouse input container.
			 */
			XENON_NODISCARD const Mouse& getMouse() const { return m_Mouse; }

			/**
			 * Get the keyboard inputs.
			 *
			 * @return The keyboard input container.
			 */
			XENON_NODISCARD const Keyboard& getKeyboard() const { return m_Keyboard; }

			/**
			 * Get the general events.
			 *
			 * @return The general events container.
			 */
			XENON_NODISCARD const GeneralEvents& getGeneralEvents() const { return m_GeneralEvents; }

		protected:
			std::string m_Title;

			uint32_t m_Width = 0;
			uint32_t m_Height = 0;

			Mouse m_Mouse;
			Keyboard m_Keyboard;
			GeneralEvents m_GeneralEvents;
		};
	}
}