// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "WindowsWindow.hpp"

#include "../XenonCore/Logging.hpp"

constexpr wchar_t g_ClassName[] = L"Xenon Windows Window Class";

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
		switch (uMsg)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_PAINT:
			return OnPaintEvent(hwnd);

		default:
			break;
		}

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
		}

		void WindowsWindow::update()
		{
			if (MSG message = {}; GetMessage(&message, m_WindowHandle, 0, 0))
			{
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
		}
	}
}