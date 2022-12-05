// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Common.hpp"

namespace Xenon
{
	/**
	 * Mouse position structure.
	 */
	struct MousePosition final
	{
		float m_XAxis = 0.0f;
		float m_YAxis = 0.0f;
	};

	/**
	 * Mouse button event enum.
	 */
	enum class MouseButtonEvent : uint8_t
	{
		Release,
		Press,
		DoublePress
	};

	/**
	 * Mouse structure.
	 * This contains all the current events of a mouse.
	 */
	struct Mouse final
	{
		MousePosition m_MousePosition;

		float m_VScroll = 0.0;
		float m_HScroll = 0.0;

		MouseButtonEvent m_ButtonLeft : 2 = MouseButtonEvent::Release;
		MouseButtonEvent m_ButtonRight : 2 = MouseButtonEvent::Release;
		MouseButtonEvent m_ButtonMiddle : 2 = MouseButtonEvent::Release;

		MouseButtonEvent m_ButtonAux1 : 2 = MouseButtonEvent::Release;
		MouseButtonEvent m_ButtonAux2 : 2 = MouseButtonEvent::Release;
		MouseButtonEvent m_ButtonAux3 : 2 = MouseButtonEvent::Release;
		MouseButtonEvent m_ButtonAux4 : 2 = MouseButtonEvent::Release;
		MouseButtonEvent m_ButtonAux5 : 2 = MouseButtonEvent::Release;
	};
}
