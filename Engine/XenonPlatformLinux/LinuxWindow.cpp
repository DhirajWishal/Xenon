// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "LinuxWindow.hpp"

#include "../XenonCore/Logging.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <optick.h>

namespace Xenon
{
	namespace Platform
	{
		LinuxWindow::LinuxWindow(const std::string& title, uint32_t width, uint32_t height)
			: Window(title, width, height)
		{
			// Create the window.
			m_pWindow = SDL_CreateWindow(title.data(), width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_FULLSCREEN);

			// Check if the window was created properly.
			if (!m_pWindow)
			{
				XENON_LOG_FATAL("Failed to create the window!");
				return;
			}

			// Set this class as user data.
			SDL_SetWindowData(m_pWindow, "this", this);
		}

		LinuxWindow::~LinuxWindow()
		{
			while (isOpen()) update();
		}

		void LinuxWindow::update()
		{
			OPTICK_EVENT();

			m_Keyboard.m_Character = 0;
			m_Mouse.m_VScroll = 0.0f;
			m_Mouse.m_HScroll = 0.0f;

			SDL_Event events;
			while (SDL_PollEvent(&events))
			{
				// Handle the events.
				switch (static_cast<SDL_EventType>(events.type))
				{
				case SDL_EVENT_FIRST:
					break;

				case SDL_EVENT_QUIT:
				case SDL_EVENT_TERMINATING:
					m_IsOpen = false;
					break;

				case SDL_EVENT_LOW_MEMORY:
				case SDL_EVENT_WILL_ENTER_BACKGROUND:
				case SDL_EVENT_DID_ENTER_BACKGROUND:
				case SDL_EVENT_WILL_ENTER_FOREGROUND:
				case SDL_EVENT_DID_ENTER_FOREGROUND:
				case SDL_EVENT_LOCALE_CHANGED:
				case SDL_EVENT_SYSTEM_THEME_CHANGED:
				case SDL_EVENT_DISPLAY_ORIENTATION:
				case SDL_EVENT_DISPLAY_CONNECTED:
				case SDL_EVENT_DISPLAY_DISCONNECTED:
				case SDL_EVENT_DISPLAY_MOVED:
				case SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED:
					// case SDL_EVENT_DISPLAY_FIRST:
					// case SDL_EVENT_DISPLAY_LAST:
				case SDL_EVENT_SYSWM:
				case SDL_EVENT_WINDOW_SHOWN:
				case SDL_EVENT_WINDOW_HIDDEN:
				case SDL_EVENT_WINDOW_EXPOSED:
				case SDL_EVENT_WINDOW_MOVED:
				case SDL_EVENT_WINDOW_RESIZED:
				case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
				case SDL_EVENT_WINDOW_MINIMIZED:
				case SDL_EVENT_WINDOW_MAXIMIZED:
				case SDL_EVENT_WINDOW_RESTORED:
				case SDL_EVENT_WINDOW_MOUSE_ENTER:
				case SDL_EVENT_WINDOW_MOUSE_LEAVE:
				case SDL_EVENT_WINDOW_FOCUS_GAINED:
				case SDL_EVENT_WINDOW_FOCUS_LOST:
				case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
				case SDL_EVENT_WINDOW_TAKE_FOCUS:
				case SDL_EVENT_WINDOW_HIT_TEST:
				case SDL_EVENT_WINDOW_ICCPROF_CHANGED:
				case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
				case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
				case SDL_EVENT_WINDOW_DESTROYED:
					// case SDL_EVENT_WINDOW_FIRST:
					// case SDL_EVENT_WINDOW_LAST:
					break;

				case SDL_EVENT_KEY_DOWN:
					handleKeyEvent(events.key.keysym.scancode, true);
					handleSpecialCharacter(events.key.keysym.mod, true);
					break;

				case SDL_EVENT_KEY_UP:
					handleKeyEvent(events.key.keysym.scancode, false);
					handleSpecialCharacter(events.key.keysym.mod, false);
					break;

				case SDL_EVENT_TEXT_EDITING:
				case SDL_EVENT_TEXT_INPUT:
				case SDL_EVENT_KEYMAP_CHANGED:
				case SDL_EVENT_TEXT_EDITING_EXT:
				case SDL_EVENT_MOUSE_MOTION:
					break;

				case SDL_EVENT_MOUSE_BUTTON_DOWN:
					handleMouseButtonEvent(events.button.button, events.button.clicks);
					break;

				case SDL_EVENT_MOUSE_BUTTON_UP:
					handleMouseButtonEvent(events.button.button, 0);
					break;

				case SDL_EVENT_MOUSE_WHEEL:
					m_Mouse.m_HScroll = events.wheel.x;
					m_Mouse.m_VScroll = events.wheel.y;
					break;

				case SDL_EVENT_JOYSTICK_AXIS_MOTION:
				case SDL_EVENT_JOYSTICK_HAT_MOTION:
				case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
				case SDL_EVENT_JOYSTICK_BUTTON_UP:
				case SDL_EVENT_JOYSTICK_ADDED:
				case SDL_EVENT_JOYSTICK_REMOVED:
				case SDL_EVENT_JOYSTICK_BATTERY_UPDATED:
				case SDL_EVENT_JOYSTICK_UPDATE_COMPLETE:
				case SDL_EVENT_GAMEPAD_AXIS_MOTION:
				case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
				case SDL_EVENT_GAMEPAD_BUTTON_UP:
				case SDL_EVENT_GAMEPAD_ADDED:
				case SDL_EVENT_GAMEPAD_REMOVED:
				case SDL_EVENT_GAMEPAD_REMAPPED:
				case SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN:
				case SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION:
				case SDL_EVENT_GAMEPAD_TOUCHPAD_UP:
				case SDL_EVENT_GAMEPAD_SENSOR_UPDATE:
				case SDL_EVENT_GAMEPAD_UPDATE_COMPLETE:
				case SDL_EVENT_FINGER_DOWN:
				case SDL_EVENT_FINGER_UP:
				case SDL_EVENT_FINGER_MOTION:
				case SDL_EVENT_CLIPBOARD_UPDATE:
				case SDL_EVENT_CLIPBOARD_CANCELLED:
				case SDL_EVENT_DROP_FILE:
				case SDL_EVENT_DROP_TEXT:
				case SDL_EVENT_DROP_BEGIN:
				case SDL_EVENT_DROP_COMPLETE:
				case SDL_EVENT_DROP_POSITION:
				case SDL_EVENT_AUDIO_DEVICE_ADDED:
				case SDL_EVENT_AUDIO_DEVICE_REMOVED:
				case SDL_EVENT_SENSOR_UPDATE:
				case SDL_EVENT_RENDER_TARGETS_RESET:
				case SDL_EVENT_RENDER_DEVICE_RESET:
				case SDL_EVENT_POLL_SENTINEL:
				case SDL_EVENT_USER:
				case SDL_EVENT_LAST:
				default:
					break;
				}
			}
		}

		bool LinuxWindow::isOpen() const
		{
			return m_IsOpen;
		}

		void LinuxWindow::handleKeyEvent(int32_t scanCode, bool pressed)
		{
			switch (scanCode)
			{
			case SDL_SCANCODE_RETURN:			m_Keyboard.m_Enter = pressed; break;
			case SDL_SCANCODE_ESCAPE:			m_Keyboard.m_Escape = pressed; break;
			case SDL_SCANCODE_BACKSPACE:		m_Keyboard.m_Backspace = pressed; break;
			case SDL_SCANCODE_TAB:				m_Keyboard.m_Tab = pressed; break;
			case SDL_SCANCODE_SPACE:			m_Keyboard.m_Space = pressed; break;

			case SDL_SCANCODE_A:				m_Keyboard.m_KeyA = pressed; break;
			case SDL_SCANCODE_B:				m_Keyboard.m_KeyB = pressed; break;
			case SDL_SCANCODE_C:				m_Keyboard.m_KeyC = pressed; break;
			case SDL_SCANCODE_D:				m_Keyboard.m_KeyD = pressed; break;
			case SDL_SCANCODE_E:				m_Keyboard.m_KeyE = pressed; break;
			case SDL_SCANCODE_F:				m_Keyboard.m_KeyF = pressed; break;
			case SDL_SCANCODE_G:				m_Keyboard.m_KeyG = pressed; break;
			case SDL_SCANCODE_H:				m_Keyboard.m_KeyH = pressed; break;
			case SDL_SCANCODE_I:				m_Keyboard.m_KeyI = pressed; break;
			case SDL_SCANCODE_J:				m_Keyboard.m_KeyJ = pressed; break;
			case SDL_SCANCODE_K:				m_Keyboard.m_KeyK = pressed; break;
			case SDL_SCANCODE_L:				m_Keyboard.m_KeyL = pressed; break;
			case SDL_SCANCODE_M:				m_Keyboard.m_KeyM = pressed; break;
			case SDL_SCANCODE_N:				m_Keyboard.m_KeyN = pressed; break;
			case SDL_SCANCODE_O:				m_Keyboard.m_KeyO = pressed; break;
			case SDL_SCANCODE_P:				m_Keyboard.m_KeyP = pressed; break;
			case SDL_SCANCODE_Q:				m_Keyboard.m_KeyQ = pressed; break;
			case SDL_SCANCODE_R:				m_Keyboard.m_KeyR = pressed; break;
			case SDL_SCANCODE_S:				m_Keyboard.m_KeyS = pressed; break;
			case SDL_SCANCODE_T:				m_Keyboard.m_KeyT = pressed; break;
			case SDL_SCANCODE_U:				m_Keyboard.m_KeyU = pressed; break;
			case SDL_SCANCODE_V:				m_Keyboard.m_KeyV = pressed; break;
			case SDL_SCANCODE_W:				m_Keyboard.m_KeyW = pressed; break;
			case SDL_SCANCODE_X:				m_Keyboard.m_KeyX = pressed; break;
			case SDL_SCANCODE_Y:				m_Keyboard.m_KeyY = pressed; break;
			case SDL_SCANCODE_Z:				m_Keyboard.m_KeyZ = pressed; break;

			case SDL_SCANCODE_1:				m_Keyboard.m_KeyOne = pressed; break;
			case SDL_SCANCODE_2:				m_Keyboard.m_KeyTwo = pressed; break;
			case SDL_SCANCODE_3:				m_Keyboard.m_KeyThree = pressed; break;
			case SDL_SCANCODE_4:				m_Keyboard.m_KeyFour = pressed; break;
			case SDL_SCANCODE_5:				m_Keyboard.m_KeyFive = pressed; break;
			case SDL_SCANCODE_6:				m_Keyboard.m_KeySix = pressed; break;
			case SDL_SCANCODE_7:				m_Keyboard.m_KeySeven = pressed; break;
			case SDL_SCANCODE_8:				m_Keyboard.m_KeyEight = pressed; break;
			case SDL_SCANCODE_9:				m_Keyboard.m_KeyNine = pressed; break;
			case SDL_SCANCODE_0:				m_Keyboard.m_KeyZero = pressed; break;

			case SDL_SCANCODE_MINUS:			m_Keyboard.m_Minus = pressed; break;
			case SDL_SCANCODE_EQUALS:			m_Keyboard.m_Equal = pressed; break;
			case SDL_SCANCODE_LEFTBRACKET:		m_Keyboard.m_LeftBracket = pressed; break;
			case SDL_SCANCODE_RIGHTBRACKET:		m_Keyboard.m_RightBracket = pressed; break;
			case SDL_SCANCODE_BACKSLASH:		m_Keyboard.m_Backslash = pressed; break;

			case SDL_SCANCODE_SEMICOLON:		m_Keyboard.m_Semicolon = pressed; break;
			case SDL_SCANCODE_APOSTROPHE:		m_Keyboard.m_Apostrophe = pressed; break;
			case SDL_SCANCODE_GRAVE:			m_Keyboard.m_GraveAccent = pressed; break;

			case SDL_SCANCODE_COMMA:			m_Keyboard.m_Comma = pressed; break;
			case SDL_SCANCODE_PERIOD:			m_Keyboard.m_Period = pressed; break;
			case SDL_SCANCODE_SLASH:			m_Keyboard.m_Slash = pressed; break;

			case SDL_SCANCODE_CAPSLOCK:			m_Keyboard.m_CapsLock = pressed; break;

			case SDL_SCANCODE_F1:				m_Keyboard.m_F1 = pressed; break;
			case SDL_SCANCODE_F2:				m_Keyboard.m_F2 = pressed; break;
			case SDL_SCANCODE_F3:				m_Keyboard.m_F3 = pressed; break;
			case SDL_SCANCODE_F4:				m_Keyboard.m_F4 = pressed; break;
			case SDL_SCANCODE_F5:				m_Keyboard.m_F5 = pressed; break;
			case SDL_SCANCODE_F6:				m_Keyboard.m_F6 = pressed; break;
			case SDL_SCANCODE_F7:				m_Keyboard.m_F7 = pressed; break;
			case SDL_SCANCODE_F8:				m_Keyboard.m_F8 = pressed; break;
			case SDL_SCANCODE_F9:				m_Keyboard.m_F9 = pressed; break;
			case SDL_SCANCODE_F10:				m_Keyboard.m_F10 = pressed; break;
			case SDL_SCANCODE_F11:				m_Keyboard.m_F11 = pressed; break;
			case SDL_SCANCODE_F12:				m_Keyboard.m_F12 = pressed; break;

			case SDL_SCANCODE_PRINTSCREEN:		m_Keyboard.m_PrintScreen = pressed; break;
			case SDL_SCANCODE_SCROLLLOCK:		m_Keyboard.m_ScrollLock = pressed; break;
			case SDL_SCANCODE_PAUSE:			m_Keyboard.m_Pause = pressed; break;
			case SDL_SCANCODE_INSERT:			m_Keyboard.m_Insert = pressed; break;

			case SDL_SCANCODE_HOME:				m_Keyboard.m_Home = pressed; break;
			case SDL_SCANCODE_PAGEUP:			m_Keyboard.m_PageUp = pressed; break;
			case SDL_SCANCODE_DELETE:			m_Keyboard.m_PageDown = pressed; break;
			case SDL_SCANCODE_END:				m_Keyboard.m_End = pressed; break;
			case SDL_SCANCODE_PAGEDOWN:			m_Keyboard.m_PageDown = pressed; break;
			case SDL_SCANCODE_RIGHT:			m_Keyboard.m_Right = pressed; break;
			case SDL_SCANCODE_LEFT:				m_Keyboard.m_Left = pressed; break;
			case SDL_SCANCODE_DOWN:				m_Keyboard.m_Down = pressed; break;
			case SDL_SCANCODE_UP:				m_Keyboard.m_Up = pressed; break;

			case SDL_SCANCODE_NUMLOCKCLEAR:		m_Keyboard.m_NumLock = pressed; break;

			case SDL_SCANCODE_KP_DIVIDE:		m_Keyboard.m_KeyPadDivide = pressed; break;
			case SDL_SCANCODE_KP_MULTIPLY:		m_Keyboard.m_KeyPadMultiply = pressed; break;
			case SDL_SCANCODE_KP_MINUS:			m_Keyboard.m_KeyPadSubtract = pressed; break;
			case SDL_SCANCODE_KP_PLUS:			m_Keyboard.m_KeyPadAdd = pressed; break;
			case SDL_SCANCODE_KP_ENTER:			m_Keyboard.m_KeyPadEnter = pressed; break;
			case SDL_SCANCODE_KP_1:				m_Keyboard.m_KeyPadOne = pressed; break;
			case SDL_SCANCODE_KP_2:				m_Keyboard.m_KeyPadTwo = pressed; break;
			case SDL_SCANCODE_KP_3:				m_Keyboard.m_KeyPadThree = pressed; break;
			case SDL_SCANCODE_KP_4:				m_Keyboard.m_KeyPadFour = pressed; break;
			case SDL_SCANCODE_KP_5:				m_Keyboard.m_KeyPadFive = pressed; break;
			case SDL_SCANCODE_KP_6:				m_Keyboard.m_KeyPadSix = pressed; break;
			case SDL_SCANCODE_KP_7:				m_Keyboard.m_KeyPadSeven = pressed; break;
			case SDL_SCANCODE_KP_8:				m_Keyboard.m_KeyPadEight = pressed; break;
			case SDL_SCANCODE_KP_9:				m_Keyboard.m_KeyPadNine = pressed; break;
			case SDL_SCANCODE_KP_0:				m_Keyboard.m_KeyPadZero = pressed; break;
			case SDL_SCANCODE_KP_PERIOD:		m_Keyboard.m_KeyPadDecimal = pressed; break;

			default:							break;
			}
		}

		void LinuxWindow::handleSpecialCharacter(uint64_t characters, bool pressed)
		{
			if (characters & SDL_KMOD_LCTRL) m_Keyboard.m_LeftControl = pressed;
			if (characters & SDL_KMOD_RCTRL) m_Keyboard.m_RightControl = pressed;
			if (characters & SDL_KMOD_LSHIFT) m_Keyboard.m_LeftShift = pressed;
			if (characters & SDL_KMOD_RSHIFT) m_Keyboard.m_RightShift = pressed;
			if (characters & SDL_KMOD_LALT) m_Keyboard.m_LeftAlt = pressed;
			if (characters & SDL_KMOD_RALT) m_Keyboard.m_RightAlt = pressed;
			if (characters & SDL_KMOD_LGUI) m_Keyboard.m_LeftSuper = pressed;
			if (characters & SDL_KMOD_RGUI) m_Keyboard.m_RightSuper = pressed;
			if (characters & SDL_KMOD_NUM) m_Keyboard.m_NumLock = pressed;
			if (characters & SDL_KMOD_CAPS) m_Keyboard.m_CapsLock = pressed;
			if (characters & SDL_KMOD_SCROLL) m_Keyboard.m_ScrollLock = pressed;
		}

		void LinuxWindow::handleMouseButtonEvent(uint8_t button, uint8_t clicks)
		{
			const auto buttonEvent = clicks == 0 ? MouseButtonEvent::Release : clicks == 1 ? MouseButtonEvent::Press : MouseButtonEvent::DoublePress;

			if (button == SDL_BUTTON_LEFT) m_Mouse.m_ButtonLeft = buttonEvent;
			else if (button == SDL_BUTTON_RIGHT) m_Mouse.m_ButtonRight = buttonEvent;
			else if (button == SDL_BUTTON_MIDDLE) m_Mouse.m_ButtonMiddle = buttonEvent;
			else if (button == SDL_BUTTON_X1) m_Mouse.m_ButtonAux1 = buttonEvent;
			else if (button == SDL_BUTTON_X2) m_Mouse.m_ButtonAux2 = buttonEvent;
		}
	}
}