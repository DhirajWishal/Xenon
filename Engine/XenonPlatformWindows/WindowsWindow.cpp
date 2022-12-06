// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "WindowsWindow.hpp"

#include "../XenonCore/Logging.hpp"

#include <optick.h>
#include <windowsx.h>

constexpr const auto* g_ClassName = TEXT("Xenon Windows Window Class");

namespace /* anonymous */
{
	/**
	 * On paint event.
	 * This function is used to pain on the window upon requested.
	 *
	 * @param hwnd The window handle.
	 * @return The result.
	 */
	LRESULT OnPaintEvent(HWND hwnd)
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		// All painting occurs here, between BeginPaint and EndPaint.
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		EndPaint(hwnd, &ps);

		return 0;
	}

	// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nc-winuser-wndproc
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		auto userPtr = GetProp(hwnd, TEXT("WindowsWindow"));
		if (userPtr)
			return std::bit_cast<Xenon::Platform::WindowsWindow*>(userPtr)->handleEvent(uMsg, wParam, lParam);

		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	/**
	 * Convert a normal string to a wide string.
	 *
	 * @param string The string to convert.
	 * @return The converted wide string.
	 */
	std::wstring ToWideString(const std::string_view& string)
	{
		std::wstring wideString;
		wideString.resize(MultiByteToWideChar(CP_ACP, 0, string.data(), static_cast<int>(string.size()), nullptr, 0));
		MultiByteToWideChar(CP_ACP, 0, string.data(), static_cast<int>(string.size()), wideString.data(), static_cast<int>(wideString.size()));

		return wideString;
	}
}

namespace Xenon
{
	namespace Platform
	{
		WindowsWindow::WindowsWindow(const std::string& title, uint32_t width, uint32_t height)
			: Window(title, width, height)
		{
			// Register the window class.
			WNDCLASS wc = { };
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = WindowProc;
			wc.hInstance = nullptr;
			wc.lpszClassName = g_ClassName;

			if (FAILED(RegisterClass(&wc)))
			{
				XENON_LOG_ERROR("Failed to register the window class!");
				return;
			}

			std::wstring wideString = ToWideString(title);

			// Create the window.
			m_WindowHandle = CreateWindowEx(
				0,								// Optional window styles. https://learn.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
				g_ClassName,					// Window class
				wideString.data(),				// Window text
				WS_OVERLAPPEDWINDOW,			// Window style
				CW_USEDEFAULT,					// X-Position.
				CW_USEDEFAULT,					// Y-Position.
				width,							// Width of the window.
				height,							// Height of the window.
				nullptr,						// Parent window    
				nullptr,						// Menu
				nullptr,						// Instance handle
				this							// Additional application data
			);

			// Get the actual window size.
			if (RECT rectangle = {}; GetClientRect(m_WindowHandle, &rectangle))
			{
				m_Width = rectangle.right;
				m_Height = rectangle.bottom;
			}

			// Set the windows window property.
			SetProp(m_WindowHandle, TEXT("WindowsWindow"), this);

			// Validate if we were able to create the window.
			if (m_WindowHandle == nullptr)
			{
				XENON_LOG_FATAL("Failed to create the window!");
				return;
			}

			// Show the window to the user.
			ShowWindow(m_WindowHandle, SW_SHOWNORMAL);
		}

		WindowsWindow::~WindowsWindow()
		{
			if (DestroyWindow(m_WindowHandle) == 0)
				XENON_LOG_ERROR("Failed to destroy the window!");

			while (isOpen()) update();
		}

		void WindowsWindow::update()
		{
			OPTICK_EVENT();

			m_Keyboard.m_Character = 0;
			m_Mouse.m_VScroll = 0.0f;
			m_Mouse.m_HScroll = 0.0f;

			if (MSG message = {}; PeekMessage(&message, m_WindowHandle, NULL, NULL, PM_REMOVE))
			{
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
		}

		bool WindowsWindow::isOpen() const
		{
			return m_IsOpen;
		}

		LRESULT WindowsWindow::handleEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			switch (uMsg)
			{
			case WM_DESTROY:
			case WM_CLOSE:
				m_IsOpen = false;
				PostQuitMessage(0);
				return 0;

			case WM_PAINT:
				if (m_OnPaintCallback)
					m_OnPaintCallback();

				return OnPaintEvent(m_WindowHandle);

			case WM_LBUTTONDBLCLK:
				m_Mouse.m_ButtonLeft = MouseButtonEvent::DoublePress;
				return 0;

			case WM_LBUTTONDOWN:
				m_Mouse.m_ButtonLeft = MouseButtonEvent::Press;
				return 0;

			case WM_LBUTTONUP:
				m_Mouse.m_ButtonLeft = MouseButtonEvent::Release;
				return 0;

			case WM_MBUTTONDBLCLK:
				m_Mouse.m_ButtonMiddle = MouseButtonEvent::DoublePress;
				return 0;

			case WM_MBUTTONDOWN:
				m_Mouse.m_ButtonMiddle = MouseButtonEvent::Press;
				return 0;

			case WM_MBUTTONUP:
				m_Mouse.m_ButtonMiddle = MouseButtonEvent::Release;
				return 0;

			case WM_RBUTTONDBLCLK:
				m_Mouse.m_ButtonRight = MouseButtonEvent::DoublePress;
				return 0;

			case WM_RBUTTONDOWN:
				m_Mouse.m_ButtonRight = MouseButtonEvent::Press;
				return 0;

			case WM_RBUTTONUP:
				m_Mouse.m_ButtonRight = MouseButtonEvent::Release;
				return 0;

			case WM_MOUSEMOVE:
			{
				if (wParam & MK_CONTROL) m_Keyboard.m_LeftControl = m_Keyboard.m_RightControl = true;
				if (wParam & MK_LBUTTON) m_Mouse.m_ButtonLeft = MouseButtonEvent::Press;
				if (wParam & MK_MBUTTON) m_Mouse.m_ButtonMiddle = MouseButtonEvent::Press;
				if (wParam & MK_RBUTTON) m_Mouse.m_ButtonRight = MouseButtonEvent::Press;
				if (wParam & MK_SHIFT) m_Keyboard.m_LeftShift = m_Keyboard.m_RightShift = true;

				m_Mouse.m_MousePosition.m_XAxis = static_cast<float>(GET_X_LPARAM(lParam));
				m_Mouse.m_MousePosition.m_YAxis = static_cast<float>(GET_Y_LPARAM(lParam));
				return 0;
			}

			case WM_MOUSEWHEEL:
			{
				if (wParam & MK_CONTROL) m_Keyboard.m_LeftControl = m_Keyboard.m_RightControl = true;
				if (wParam & MK_LBUTTON) m_Mouse.m_ButtonLeft = MouseButtonEvent::Press;
				if (wParam & MK_MBUTTON) m_Mouse.m_ButtonMiddle = MouseButtonEvent::Press;
				if (wParam & MK_RBUTTON) m_Mouse.m_ButtonRight = MouseButtonEvent::Press;
				if (wParam & MK_SHIFT) m_Keyboard.m_LeftShift = m_Keyboard.m_RightShift = true;

				m_Mouse.m_MousePosition.m_XAxis = static_cast<float>(GET_X_LPARAM(lParam));
				m_Mouse.m_MousePosition.m_YAxis = static_cast<float>(GET_Y_LPARAM(lParam));

				m_Mouse.m_VScroll = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA;
				return 0;
			}

			case WM_MOUSEHWHEEL:
			{
				if (wParam & MK_CONTROL) m_Keyboard.m_LeftControl = m_Keyboard.m_RightControl = true;
				if (wParam & MK_LBUTTON) m_Mouse.m_ButtonLeft = MouseButtonEvent::Press;
				if (wParam & MK_MBUTTON) m_Mouse.m_ButtonMiddle = MouseButtonEvent::Press;
				if (wParam & MK_RBUTTON) m_Mouse.m_ButtonRight = MouseButtonEvent::Press;
				if (wParam & MK_SHIFT) m_Keyboard.m_LeftShift = m_Keyboard.m_RightShift = true;

				m_Mouse.m_MousePosition.m_XAxis = static_cast<float>(GET_X_LPARAM(lParam));
				m_Mouse.m_MousePosition.m_YAxis = static_cast<float>(GET_Y_LPARAM(lParam));

				m_Mouse.m_HScroll = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA;
				return 0;
			}

			case WM_KEYDOWN:
				switch (wParam)
				{
				case VK_LBUTTON:
					m_Mouse.m_ButtonLeft = MouseButtonEvent::Press;
					break;

				case VK_RBUTTON:
					m_Mouse.m_ButtonRight = MouseButtonEvent::Press;
					break;

				case VK_MBUTTON:
					m_Mouse.m_ButtonMiddle = MouseButtonEvent::Press;
					break;

				case VK_BACK:
					m_Keyboard.m_Backspace = true;
					break;

				case VK_TAB:
					m_Keyboard.m_Tab = true;
					break;

				case VK_RETURN:
					m_Keyboard.m_Enter = true;
					break;

				case VK_PAUSE:
					m_Keyboard.m_Pause = true;
					break;

				case VK_CAPITAL:
					m_Keyboard.m_CapsLock = true;
					break;

				case VK_ESCAPE:
					m_Keyboard.m_Escape = true;
					break;

				case VK_SPACE:
					m_Keyboard.m_Space = true;
					break;

				case VK_PRIOR:
					m_Keyboard.m_PageUp = true;
					break;

				case VK_NEXT:
					m_Keyboard.m_PageDown = true;
					break;

				case VK_END:
					m_Keyboard.m_End = true;
					break;

				case VK_HOME:
					m_Keyboard.m_Home = true;
					break;

				case VK_LEFT:
					m_Keyboard.m_Left = true;
					break;

				case VK_UP:
					m_Keyboard.m_Up = true;
					break;

				case VK_RIGHT:
					m_Keyboard.m_Right = true;
					break;

				case VK_DOWN:
					m_Keyboard.m_Down = true;
					break;

				case VK_SNAPSHOT:
					m_Keyboard.m_PrintScreen = true;
					break;

				case VK_INSERT:
					m_Keyboard.m_Insert = true;
					break;

				case VK_DELETE:
					m_Keyboard.m_Delete = true;
					break;

				case 0x30:
					m_Keyboard.m_KeyZero = true;
					break;

				case 0x31:
					m_Keyboard.m_KeyOne = true;
					break;

				case 0x32:
					m_Keyboard.m_KeyTwo = true;
					break;

				case 0x33:
					m_Keyboard.m_KeyThree = true;
					break;

				case 0x34:
					m_Keyboard.m_KeyFour = true;
					break;

				case 0x35:
					m_Keyboard.m_KeyFive = true;
					break;

				case 0x36:
					m_Keyboard.m_KeySix = true;
					break;

				case 0x37:
					m_Keyboard.m_KeySeven = true;
					break;

				case 0x38:
					m_Keyboard.m_KeyEight = true;
					break;

				case 0x39:
					m_Keyboard.m_KeyNine = true;
					break;

				case VK_LWIN:
					m_Keyboard.m_LeftSuper = true;
					break;

				case VK_RWIN:
					m_Keyboard.m_RightSuper = true;
					break;

				case VK_NUMPAD0:
					m_Keyboard.m_KeyPadZero = true;
					break;

				case VK_NUMPAD1:
					m_Keyboard.m_KeyPadOne = true;
					break;

				case VK_NUMPAD2:
					m_Keyboard.m_KeyPadTwo = true;
					break;

				case VK_NUMPAD3:
					m_Keyboard.m_KeyPadThree = true;
					break;

				case VK_NUMPAD4:
					m_Keyboard.m_KeyPadFour = true;
					break;

				case VK_NUMPAD5:
					m_Keyboard.m_KeyPadFive = true;
					break;

				case VK_NUMPAD6:
					m_Keyboard.m_KeyPadSix = true;
					break;

				case VK_NUMPAD7:
					m_Keyboard.m_KeyPadSeven = true;
					break;

				case VK_NUMPAD8:
					m_Keyboard.m_KeyPadEight = true;
					break;

				case VK_NUMPAD9:
					m_Keyboard.m_KeyPadNine = true;
					break;

				case VK_MULTIPLY:
					m_Keyboard.m_KeyPadMultiply = true;
					break;

				case VK_ADD:
					m_Keyboard.m_KeyPadAdd = true;
					break;

				case VK_SUBTRACT:
					m_Keyboard.m_KeyPadSubtract = true;
					break;

				case VK_DECIMAL:
					m_Keyboard.m_KeyPadDecimal = true;
					break;

				case VK_DIVIDE:
					m_Keyboard.m_KeyPadDivide = true;
					break;

				case VK_F1:
					m_Keyboard.m_F1 = true;
					break;

				case VK_F2:
					m_Keyboard.m_F2 = true;
					break;

				case VK_F3:
					m_Keyboard.m_F3 = true;
					break;

				case VK_F4:
					m_Keyboard.m_F4 = true;
					break;

				case VK_F5:
					m_Keyboard.m_F5 = true;
					break;

				case VK_F6:
					m_Keyboard.m_F6 = true;
					break;

				case VK_F7:
					m_Keyboard.m_F7 = true;
					break;

				case VK_F8:
					m_Keyboard.m_F8 = true;
					break;

				case VK_F9:
					m_Keyboard.m_F9 = true;
					break;

				case VK_F10:
					m_Keyboard.m_F10 = true;
					break;

				case VK_F11:
					m_Keyboard.m_F11 = true;
					break;

				case VK_F12:
					m_Keyboard.m_F12 = true;
					break;

				case VK_F13:
					m_Keyboard.m_F13 = true;
					break;

				case VK_F14:
					m_Keyboard.m_F14 = true;
					break;

				case VK_F15:
					m_Keyboard.m_F15 = true;
					break;

				case VK_F16:
					m_Keyboard.m_F16 = true;
					break;

				case VK_F17:
					m_Keyboard.m_F17 = true;
					break;

				case VK_F18:
					m_Keyboard.m_F18 = true;
					break;

				case VK_F19:
					m_Keyboard.m_F19 = true;
					break;

				case VK_F20:
					m_Keyboard.m_F20 = true;
					break;

				case VK_F21:
					m_Keyboard.m_F21 = true;
					break;

				case VK_F22:
					m_Keyboard.m_F22 = true;
					break;

				case VK_F23:
					m_Keyboard.m_F23 = true;
					break;

				case VK_F24:
					m_Keyboard.m_F24 = true;
					break;

				case VK_NUMLOCK:
					m_Keyboard.m_NumLock = true;
					break;

				case VK_SCROLL:
					m_Keyboard.m_ScrollLock = true;
					break;

				case VK_LSHIFT:
					m_Keyboard.m_LeftShift = true;
					break;

				case VK_RSHIFT:
					m_Keyboard.m_RightShift = true;
					break;

				case VK_LCONTROL:
					m_Keyboard.m_LeftControl = true;
					break;

				case VK_RCONTROL:
					m_Keyboard.m_RightControl = true;
					break;

				case VK_LMENU:
				case VK_RMENU:
					m_Keyboard.m_Menu = true;
					break;

				default:
					break;
				}

				return 0;

			case WM_KEYUP:
				switch (wParam)
				{
				case VK_LBUTTON:
					m_Mouse.m_ButtonLeft = MouseButtonEvent::Release;
					break;

				case VK_RBUTTON:
					m_Mouse.m_ButtonRight = MouseButtonEvent::Release;
					break;

				case VK_MBUTTON:
					m_Mouse.m_ButtonMiddle = MouseButtonEvent::Release;
					break;

				case VK_BACK:
					m_Keyboard.m_Backspace = false;
					break;

				case VK_TAB:
					m_Keyboard.m_Tab = false;
					break;

				case VK_RETURN:
					m_Keyboard.m_Enter = false;
					break;

				case VK_PAUSE:
					m_Keyboard.m_Pause = false;
					break;

				case VK_CAPITAL:
					m_Keyboard.m_CapsLock = false;
					break;

				case VK_ESCAPE:
					m_Keyboard.m_Escape = false;
					break;

				case VK_SPACE:
					m_Keyboard.m_Space = false;
					break;

				case VK_PRIOR:
					m_Keyboard.m_PageUp = false;
					break;

				case VK_NEXT:
					m_Keyboard.m_PageDown = false;
					break;

				case VK_END:
					m_Keyboard.m_End = false;
					break;

				case VK_HOME:
					m_Keyboard.m_Home = false;
					break;

				case VK_LEFT:
					m_Keyboard.m_Left = false;
					break;

				case VK_UP:
					m_Keyboard.m_Up = false;
					break;

				case VK_RIGHT:
					m_Keyboard.m_Right = false;
					break;

				case VK_DOWN:
					m_Keyboard.m_Down = false;
					break;

				case VK_SNAPSHOT:
					m_Keyboard.m_PrintScreen = false;
					break;

				case VK_INSERT:
					m_Keyboard.m_Insert = false;
					break;

				case VK_DELETE:
					m_Keyboard.m_Delete = false;
					break;

				case 0x30:
					m_Keyboard.m_KeyZero = false;
					break;

				case 0x31:
					m_Keyboard.m_KeyOne = false;
					break;

				case 0x32:
					m_Keyboard.m_KeyTwo = false;
					break;

				case 0x33:
					m_Keyboard.m_KeyThree = false;
					break;

				case 0x34:
					m_Keyboard.m_KeyFour = false;
					break;

				case 0x35:
					m_Keyboard.m_KeyFive = false;
					break;

				case 0x36:
					m_Keyboard.m_KeySix = false;
					break;

				case 0x37:
					m_Keyboard.m_KeySeven = false;
					break;

				case 0x38:
					m_Keyboard.m_KeyEight = false;
					break;

				case 0x39:
					m_Keyboard.m_KeyNine = false;
					break;

				case VK_LWIN:
					m_Keyboard.m_LeftSuper = false;
					break;

				case VK_RWIN:
					m_Keyboard.m_RightSuper = false;
					break;

				case VK_NUMPAD0:
					m_Keyboard.m_KeyPadZero = false;
					break;

				case VK_NUMPAD1:
					m_Keyboard.m_KeyPadOne = false;
					break;

				case VK_NUMPAD2:
					m_Keyboard.m_KeyPadTwo = false;
					break;

				case VK_NUMPAD3:
					m_Keyboard.m_KeyPadThree = false;
					break;

				case VK_NUMPAD4:
					m_Keyboard.m_KeyPadFour = false;
					break;

				case VK_NUMPAD5:
					m_Keyboard.m_KeyPadFive = false;
					break;

				case VK_NUMPAD6:
					m_Keyboard.m_KeyPadSix = false;
					break;

				case VK_NUMPAD7:
					m_Keyboard.m_KeyPadSeven = false;
					break;

				case VK_NUMPAD8:
					m_Keyboard.m_KeyPadEight = false;
					break;

				case VK_NUMPAD9:
					m_Keyboard.m_KeyPadNine = false;
					break;

				case VK_MULTIPLY:
					m_Keyboard.m_KeyPadMultiply = false;
					break;

				case VK_ADD:
					m_Keyboard.m_KeyPadAdd = false;
					break;

				case VK_SUBTRACT:
					m_Keyboard.m_KeyPadSubtract = false;
					break;

				case VK_DECIMAL:
					m_Keyboard.m_KeyPadDecimal = false;
					break;

				case VK_DIVIDE:
					m_Keyboard.m_KeyPadDivide = false;
					break;

				case VK_F1:
					m_Keyboard.m_F1 = false;
					break;

				case VK_F2:
					m_Keyboard.m_F2 = false;
					break;

				case VK_F3:
					m_Keyboard.m_F3 = false;
					break;

				case VK_F4:
					m_Keyboard.m_F4 = false;
					break;

				case VK_F5:
					m_Keyboard.m_F5 = false;
					break;

				case VK_F6:
					m_Keyboard.m_F6 = false;
					break;

				case VK_F7:
					m_Keyboard.m_F7 = false;
					break;

				case VK_F8:
					m_Keyboard.m_F8 = false;
					break;

				case VK_F9:
					m_Keyboard.m_F9 = false;
					break;

				case VK_F10:
					m_Keyboard.m_F10 = false;
					break;

				case VK_F11:
					m_Keyboard.m_F11 = false;
					break;

				case VK_F12:
					m_Keyboard.m_F12 = false;
					break;

				case VK_F13:
					m_Keyboard.m_F13 = false;
					break;

				case VK_F14:
					m_Keyboard.m_F14 = false;
					break;

				case VK_F15:
					m_Keyboard.m_F15 = false;
					break;

				case VK_F16:
					m_Keyboard.m_F16 = false;
					break;

				case VK_F17:
					m_Keyboard.m_F17 = false;
					break;

				case VK_F18:
					m_Keyboard.m_F18 = false;
					break;

				case VK_F19:
					m_Keyboard.m_F19 = false;
					break;

				case VK_F20:
					m_Keyboard.m_F20 = false;
					break;

				case VK_F21:
					m_Keyboard.m_F21 = false;
					break;

				case VK_F22:
					m_Keyboard.m_F22 = false;
					break;

				case VK_F23:
					m_Keyboard.m_F23 = false;
					break;

				case VK_F24:
					m_Keyboard.m_F24 = false;
					break;

				case VK_NUMLOCK:
					m_Keyboard.m_NumLock = false;
					break;

				case VK_SCROLL:
					m_Keyboard.m_ScrollLock = false;
					break;

				case VK_LSHIFT:
					m_Keyboard.m_LeftShift = false;
					break;

				case VK_RSHIFT:
					m_Keyboard.m_RightShift = false;
					break;

				case VK_LCONTROL:
					m_Keyboard.m_LeftControl = false;
					break;

				case VK_RCONTROL:
					m_Keyboard.m_RightControl = false;
					break;

				case VK_LMENU:
				case VK_RMENU:
					m_Keyboard.m_Menu = false;
					break;

				default:
					break;
				}

				return 0;

			case WM_CHAR:
				m_Keyboard.m_Character = static_cast<char>(wParam);
				return 0;

			case WM_SIZE:
				m_Width = LOWORD(lParam);
				m_Height = HIWORD(lParam);
				return 0;

			default:
				break;
			}

			return DefWindowProc(m_WindowHandle, uMsg, wParam, lParam);
		}
	}
}