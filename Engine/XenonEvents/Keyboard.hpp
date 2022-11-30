// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Common.hpp"

namespace Xenon
{
	/**
	 * Keyboard structure.
	 */
	struct Keyboard final
	{
		char m_Character = 0;

		bool m_Unknown : 1 = false;

		bool m_Space : 1 = false;
		bool m_Apostrophe : 1 = false;

		bool m_Comma : 1 = false;
		bool m_Minus : 1 = false;
		bool m_Period : 1 = false;
		bool m_Slash : 1 = false;

		bool m_KeyZero : 1 = false;
		bool m_KeyOne : 1 = false;
		bool m_KeyTwo : 1 = false;
		bool m_KeyThree : 1 = false;
		bool m_KeyFour : 1 = false;
		bool m_KeyFive : 1 = false;
		bool m_KeySix : 1 = false;
		bool m_KeySeven : 1 = false;
		bool m_KeyEight : 1 = false;
		bool m_KeyNine : 1 = false;

		bool m_Semicolon : 1 = false;
		bool m_Equal : 1 = false;

		bool m_LeftBracket : 1 = false;
		bool m_RightBracket : 1 = false;

		bool m_Backslash : 1 = false;
		bool m_GraveAccent : 1 = false;
		bool m_WordOne : 1 = false;
		bool m_WordTwo : 1 = false;

		bool m_Escape : 1 = false;
		bool m_Enter : 1 = false;
		bool m_Tab : 1 = false;
		bool m_Backspace : 1 = false;
		bool m_Insert : 1 = false;
		bool m_Delete : 1 = false;
		bool m_Right : 1 = false;
		bool m_Left : 1 = false;
		bool m_Down : 1 = false;
		bool m_Up : 1 = false;
		bool m_PageUp : 1 = false;
		bool m_PageDown : 1 = false;
		bool m_Home : 1 = false;
		bool m_End : 1 = false;
		bool m_CapsLock : 1 = false;
		bool m_ScrollLock : 1 = false;
		bool m_NumLock : 1 = false;
		bool m_PrintScreen : 1 = false;
		bool m_Pause : 1 = false;

		bool m_F1 : 1 = false;
		bool m_F2 : 1 = false;
		bool m_F3 : 1 = false;
		bool m_F4 : 1 = false;
		bool m_F5 : 1 = false;
		bool m_F6 : 1 = false;
		bool m_F7 : 1 = false;
		bool m_F8 : 1 = false;
		bool m_F9 : 1 = false;
		bool m_F10 : 1 = false;
		bool m_F11 : 1 = false;
		bool m_F12 : 1 = false;
		bool m_F13 : 1 = false;
		bool m_F14 : 1 = false;
		bool m_F15 : 1 = false;
		bool m_F16 : 1 = false;
		bool m_F17 : 1 = false;
		bool m_F18 : 1 = false;
		bool m_F19 : 1 = false;
		bool m_F20 : 1 = false;
		bool m_F21 : 1 = false;
		bool m_F22 : 1 = false;
		bool m_F23 : 1 = false;
		bool m_F24 : 1 = false;
		bool m_F25 : 1 = false;

		bool m_KeyPadZero : 1 = false;
		bool m_KeyPadOne : 1 = false;
		bool m_KeyPadTwo : 1 = false;
		bool m_KeyPadThree : 1 = false;
		bool m_KeyPadFour : 1 = false;
		bool m_KeyPadFive : 1 = false;
		bool m_KeyPadSix : 1 = false;
		bool m_KeyPadSeven : 1 = false;
		bool m_KeyPadEight : 1 = false;
		bool m_KeyPadNine : 1 = false;

		bool m_KeyPadDecimal : 1 = false;
		bool m_KeyPadDivide : 1 = false;
		bool m_KeyPadMultiply : 1 = false;
		bool m_KeyPadSubtract : 1 = false;
		bool m_KeyPadAdd : 1 = false;
		bool m_KeyPadEqual : 1 = false;
		bool m_KeyPadEnter : 1 = false;

		bool m_LeftShift : 1 = false;
		bool m_LeftControl : 1 = false;
		bool m_LeftAlt : 1 = false;
		bool m_LeftSuper : 1 = false;
		bool m_RightShift : 1 = false;
		bool m_RightControl : 1 = false;
		bool m_RightAlt : 1 = false;
		bool m_RightSuper : 1 = false;

		bool m_Menu : 1 = false;
	};
}