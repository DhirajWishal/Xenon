// Copyright 2022-2023 Dhiraj Wishal
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
			return XENON_BIT_CAST(Xenon::Platform::WindowsWindow*, userPtr)->handleEvent(uMsg, wParam, lParam);

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
				WS_EX_ACCEPTFILES,				// Optional window styles. https://learn.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
				g_ClassName,					// Window class
				wideString.data(),				// Window text
				WS_BORDER,						// Window style
				CW_USEDEFAULT,					// X-Position.
				CW_USEDEFAULT,					// Y-Position.
				width,							// Width of the window.
				height,							// Height of the window.
				nullptr,						// Parent window    
				nullptr,						// Menu
				nullptr,						// Instance handle
				this							// Additional application data
			);

			SetWindowLong(m_WindowHandle, GWL_STYLE, 0);

			// Set the windows window property.
			SetProp(m_WindowHandle, TEXT("WindowsWindow"), this);

			// Validate if we were able to create the window.
			if (m_WindowHandle == nullptr)
			{
				XENON_LOG_FATAL("Failed to create the window!");
				return;
			}

			// Show the window to the user.
			ShowWindow(m_WindowHandle, SW_SHOWMAXIMIZED);

			// Get the actual window size.
			if (RECT rectangle = {}; GetClientRect(m_WindowHandle, &rectangle))
			{
				m_Width = rectangle.right;
				m_Height = rectangle.bottom;
			}
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
			m_GeneralEvents.m_DragDropFiles.clear();

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
			OPTICK_EVENT();

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
				if (wParam & MK_CONTROL) m_Keyboard.m_LeftControl = m_Keyboard.m_RightControl = true;
				if (wParam & MK_LBUTTON) m_Mouse.m_ButtonLeft = MouseButtonEvent::Press;
				if (wParam & MK_MBUTTON) m_Mouse.m_ButtonMiddle = MouseButtonEvent::Press;
				if (wParam & MK_RBUTTON) m_Mouse.m_ButtonRight = MouseButtonEvent::Press;
				if (wParam & MK_SHIFT) m_Keyboard.m_LeftShift = m_Keyboard.m_RightShift = true;

				m_Mouse.m_MousePosition.m_XAxis = static_cast<float>(GET_X_LPARAM(lParam));
				m_Mouse.m_MousePosition.m_YAxis = static_cast<float>(GET_Y_LPARAM(lParam));
				return 0;

			case WM_MOUSEWHEEL:
				if (wParam & MK_CONTROL) m_Keyboard.m_LeftControl = m_Keyboard.m_RightControl = true;
				if (wParam & MK_LBUTTON) m_Mouse.m_ButtonLeft = MouseButtonEvent::Press;
				if (wParam & MK_MBUTTON) m_Mouse.m_ButtonMiddle = MouseButtonEvent::Press;
				if (wParam & MK_RBUTTON) m_Mouse.m_ButtonRight = MouseButtonEvent::Press;
				if (wParam & MK_SHIFT) m_Keyboard.m_LeftShift = m_Keyboard.m_RightShift = true;

				m_Mouse.m_VScroll = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA;
				return 0;

			case WM_MOUSEHWHEEL:
				if (wParam & MK_CONTROL) m_Keyboard.m_LeftControl = m_Keyboard.m_RightControl = true;
				if (wParam & MK_LBUTTON) m_Mouse.m_ButtonLeft = MouseButtonEvent::Press;
				if (wParam & MK_MBUTTON) m_Mouse.m_ButtonMiddle = MouseButtonEvent::Press;
				if (wParam & MK_RBUTTON) m_Mouse.m_ButtonRight = MouseButtonEvent::Press;
				if (wParam & MK_SHIFT) m_Keyboard.m_LeftShift = m_Keyboard.m_RightShift = true;

				m_Mouse.m_HScroll = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA;
				return 0;

			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
				return handleKeyInput(wParam, true);

			case WM_KEYUP:
			case WM_SYSKEYUP:
				return handleKeyInput(wParam, false);

			case WM_CHAR:
				if (lParam & KF_UP)
					m_Keyboard.m_Character = 0;

				else
					m_Keyboard.m_Character = static_cast<char>(wParam);

				return 0;

			case WM_SIZE:
				m_Width = LOWORD(lParam);
				m_Height = HIWORD(lParam);
				return 0;

			case WM_SETCURSOR:
				if (LOWORD(lParam) == HTCLIENT)
				{
					SetCursor(nullptr);
					return FALSE;
				}

				break;

			case WM_DROPFILES:
			{
				m_GeneralEvents.m_DragDropFiles.resize(DragQueryFile(XENON_BIT_CAST(HDROP, wParam), 0xFFFFFFFF, nullptr, 0));
				for (UINT i = 0; i < m_GeneralEvents.m_DragDropFiles.size(); i++)
				{
					auto& buffer = m_GeneralEvents.m_DragDropFiles[i];
					buffer.resize(DragQueryFile(XENON_BIT_CAST(HDROP, wParam), i, nullptr, 0) + 1);
					if (DragQueryFileA(XENON_BIT_CAST(HDROP, wParam), i, buffer.data(), static_cast<UINT>(buffer.size())) == 0)
						XENON_LOG_FATAL("Failed to get the drag drop file query!");
				}
			}
			break;

			default:
				break;
			}

			return DefWindowProc(m_WindowHandle, uMsg, wParam, lParam);
		}

		LRESULT WindowsWindow::handleKeyInput(WPARAM wParam, bool state)
		{
			OPTICK_EVENT();

			switch (wParam)
			{
			case VK_LBUTTON:
				m_Mouse.m_ButtonLeft = state ? MouseButtonEvent::Press : MouseButtonEvent::Release;
				break;

			case VK_RBUTTON:
				m_Mouse.m_ButtonRight = state ? MouseButtonEvent::Press : MouseButtonEvent::Release;
				break;

			case VK_MBUTTON:
				m_Mouse.m_ButtonMiddle = state ? MouseButtonEvent::Press : MouseButtonEvent::Release;
				break;

			case VK_BACK:
				m_Keyboard.m_Backspace = state;
				break;

			case VK_TAB:
				m_Keyboard.m_Tab = state;
				break;

			case VK_RETURN:
				m_Keyboard.m_Enter = state;
				break;

			case VK_PAUSE:
				m_Keyboard.m_Pause = state;
				break;

			case VK_CAPITAL:
				m_Keyboard.m_CapsLock = state;
				break;

			case VK_ESCAPE:
				m_Keyboard.m_Escape = state;
				break;

			case VK_SPACE:
				m_Keyboard.m_Space = state;
				break;

			case VK_PRIOR:
				m_Keyboard.m_PageUp = state;
				break;

			case VK_NEXT:
				m_Keyboard.m_PageDown = state;
				break;

			case VK_END:
				m_Keyboard.m_End = state;
				break;

			case VK_HOME:
				m_Keyboard.m_Home = state;
				break;

			case VK_LEFT:
				m_Keyboard.m_Left = state;
				break;

			case VK_UP:
				m_Keyboard.m_Up = state;
				break;

			case VK_RIGHT:
				m_Keyboard.m_Right = state;
				break;

			case VK_DOWN:
				m_Keyboard.m_Down = state;
				break;

			case VK_SNAPSHOT:
				m_Keyboard.m_PrintScreen = state;
				break;

			case VK_INSERT:
				m_Keyboard.m_Insert = state;
				break;

			case VK_DELETE:
				m_Keyboard.m_Delete = state;
				break;

			case 0x30:
				m_Keyboard.m_KeyZero = state;
				break;

			case 0x31:
				m_Keyboard.m_KeyOne = state;
				break;

			case 0x32:
				m_Keyboard.m_KeyTwo = state;
				break;

			case 0x33:
				m_Keyboard.m_KeyThree = state;
				break;

			case 0x34:
				m_Keyboard.m_KeyFour = state;
				break;

			case 0x35:
				m_Keyboard.m_KeyFive = state;
				break;

			case 0x36:
				m_Keyboard.m_KeySix = state;
				break;

			case 0x37:
				m_Keyboard.m_KeySeven = state;
				break;

			case 0x38:
				m_Keyboard.m_KeyEight = state;
				break;

			case 0x39:
				m_Keyboard.m_KeyNine = state;
				break;

			case 0x41:
				m_Keyboard.m_KeyA = state;
				break;

			case 0x42:
				m_Keyboard.m_KeyB = state;
				break;

			case 0x43:
				m_Keyboard.m_KeyC = state;
				break;

			case 0x44:
				m_Keyboard.m_KeyD = state;
				break;

			case 0x45:
				m_Keyboard.m_KeyE = state;
				break;

			case 0x46:
				m_Keyboard.m_KeyF = state;
				break;

			case 0x47:
				m_Keyboard.m_KeyG = state;
				break;

			case 0x48:
				m_Keyboard.m_KeyH = state;
				break;

			case 0x49:
				m_Keyboard.m_KeyI = state;
				break;

			case 0x4A:
				m_Keyboard.m_KeyJ = state;
				break;

			case 0x4B:
				m_Keyboard.m_KeyK = state;
				break;

			case 0x4C:
				m_Keyboard.m_KeyL = state;
				break;

			case 0x4D:
				m_Keyboard.m_KeyM = state;
				break;

			case 0x4E:
				m_Keyboard.m_KeyN = state;
				break;

			case 0x4F:
				m_Keyboard.m_KeyO = state;
				break;

			case 0x50:
				m_Keyboard.m_KeyP = state;
				break;

			case 0x51:
				m_Keyboard.m_KeyQ = state;
				break;

			case 0x52:
				m_Keyboard.m_KeyR = state;
				break;

			case 0x53:
				m_Keyboard.m_KeyS = state;
				break;

			case 0x54:
				m_Keyboard.m_KeyT = state;
				break;

			case 0x55:
				m_Keyboard.m_KeyU = state;
				break;

			case 0x56:
				m_Keyboard.m_KeyV = state;
				break;

			case 0x57:
				m_Keyboard.m_KeyW = state;
				break;

			case 0x58:
				m_Keyboard.m_KeyX = state;
				break;

			case 0x59:
				m_Keyboard.m_KeyY = state;
				break;

			case 0x5A:
				m_Keyboard.m_KeyZ = state;
				break;

			case VK_LWIN:
				m_Keyboard.m_LeftSuper = state;
				break;

			case VK_RWIN:
				m_Keyboard.m_RightSuper = state;
				break;

			case VK_NUMPAD0:
				m_Keyboard.m_KeyPadZero = state;
				break;

			case VK_NUMPAD1:
				m_Keyboard.m_KeyPadOne = state;
				break;

			case VK_NUMPAD2:
				m_Keyboard.m_KeyPadTwo = state;
				break;

			case VK_NUMPAD3:
				m_Keyboard.m_KeyPadThree = state;
				break;

			case VK_NUMPAD4:
				m_Keyboard.m_KeyPadFour = state;
				break;

			case VK_NUMPAD5:
				m_Keyboard.m_KeyPadFive = state;
				break;

			case VK_NUMPAD6:
				m_Keyboard.m_KeyPadSix = state;
				break;

			case VK_NUMPAD7:
				m_Keyboard.m_KeyPadSeven = state;
				break;

			case VK_NUMPAD8:
				m_Keyboard.m_KeyPadEight = state;
				break;

			case VK_NUMPAD9:
				m_Keyboard.m_KeyPadNine = state;
				break;

			case VK_MULTIPLY:
				m_Keyboard.m_KeyPadMultiply = state;
				break;

			case VK_ADD:
				m_Keyboard.m_KeyPadAdd = state;
				break;

			case VK_SUBTRACT:
				m_Keyboard.m_KeyPadSubtract = state;
				break;

			case VK_DECIMAL:
				m_Keyboard.m_KeyPadDecimal = state;
				break;

			case VK_DIVIDE:
				m_Keyboard.m_KeyPadDivide = state;
				break;

			case VK_F1:
				m_Keyboard.m_F1 = state;
				break;

			case VK_F2:
				m_Keyboard.m_F2 = state;
				break;

			case VK_F3:
				m_Keyboard.m_F3 = state;
				break;

			case VK_F4:
				m_Keyboard.m_F4 = state;
				break;

			case VK_F5:
				m_Keyboard.m_F5 = state;
				break;

			case VK_F6:
				m_Keyboard.m_F6 = state;
				break;

			case VK_F7:
				m_Keyboard.m_F7 = state;
				break;

			case VK_F8:
				m_Keyboard.m_F8 = state;
				break;

			case VK_F9:
				m_Keyboard.m_F9 = state;
				break;

			case VK_F10:
				m_Keyboard.m_F10 = state;
				break;

			case VK_F11:
				m_Keyboard.m_F11 = state;
				break;

			case VK_F12:
				m_Keyboard.m_F12 = state;
				break;

			case VK_F13:
				m_Keyboard.m_F13 = state;
				break;

			case VK_F14:
				m_Keyboard.m_F14 = state;
				break;

			case VK_F15:
				m_Keyboard.m_F15 = state;
				break;

			case VK_F16:
				m_Keyboard.m_F16 = state;
				break;

			case VK_F17:
				m_Keyboard.m_F17 = state;
				break;

			case VK_F18:
				m_Keyboard.m_F18 = state;
				break;

			case VK_F19:
				m_Keyboard.m_F19 = state;
				break;

			case VK_F20:
				m_Keyboard.m_F20 = state;
				break;

			case VK_F21:
				m_Keyboard.m_F21 = state;
				break;

			case VK_F22:
				m_Keyboard.m_F22 = state;
				break;

			case VK_F23:
				m_Keyboard.m_F23 = state;
				break;

			case VK_F24:
				m_Keyboard.m_F24 = state;
				break;

			case VK_NUMLOCK:
				m_Keyboard.m_NumLock = state;
				break;

			case VK_SCROLL:
				m_Keyboard.m_ScrollLock = state;
				break;

			case VK_LSHIFT:
				m_Keyboard.m_LeftShift = state;
				break;

			case VK_RSHIFT:
				m_Keyboard.m_RightShift = state;
				break;

			case VK_LCONTROL:
				m_Keyboard.m_LeftControl = state;
				break;

			case VK_RCONTROL:
				m_Keyboard.m_RightControl = state;
				break;

			case VK_LMENU:
			case VK_RMENU:
				m_Keyboard.m_Menu = state;
				break;

			default:
				break;
			}

			return 0;
		}
	}
}