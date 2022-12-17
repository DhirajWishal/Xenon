// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "LinuxWindow.hpp"
#include "../XenonCore/Logging.hpp"

namespace /* anonymous */
{
	/**
	 * Global registry handler function.
	 *
	 * @param pData The provided data pointer.
	 * @param pRegistry The registry pointer.
	 * @param id The ID.
	 * @param pInterface The interface pointer.
	 * @param version The version.
	 */
	void GlobalRegistryHandler(void *pData, struct wl_registry *pRegistry, uint32_t id,
							   const char *pInterface, uint32_t version)
	{
		auto pWindow = std::bit_cast<Xenon::Platform::LinuxWindow *>(pData);
		if (strcmp(pInterface, "wl_compositor") == 0)
		{
			pWindow->setCompositor(wl_registry_bind(pRegistry,
													id,
													&wl_compositor_interface,
													1));
		}
		else if (strcmp(pInterface, "wl_shell") == 0)
		{
			pWindow->setShell(wl_registry_bind(pRegistry, id,
											   &wl_shell_interface, 1));
		}
	}

	/**
	 * Global registry remover function.
	 *
	 * @param pData The provided data pointer.
	 * @param registry The registry pointer.
	 * @param id The ID.
	 */
	void GlobalRegistryRemover(void *pData, struct wl_registry *registry, uint32_t id)
	{
		XENON_LOG_INFORMATION("Got a registry losing event for {}", id);
	}
}

namespace Xenon
{
	namespace Platform
	{
		LinuxWindow::LinuxWindow(const std::string &title, uint32_t width, uint32_t height)
			: Window(title, width, height)
		{
			m_pDisplay = wl_display_connect(nullptr);
			if (m_pDisplay == nullptr)
			{
				XENON_LOG_FATAL("Failed to create the Wayland display connection!");
				return;
			}

			wl_display_dispatch(m_pDisplay);

			wl_registry *pRegistry = wl_display_get_registry(m_pDisplay);
			wl_registry_listener registryListener = {GlobalRegistryHandler, GlobalRegistryRemover};
			wl_registry_add_listener(pRegistry, &registryListener, this);

			wl_display_dispatch(m_pDisplay);
			wl_display_roundtrip(m_pDisplay);

			if (m_pCompositor == nullptr)
			{
				XENON_LOG_FATAL("Can't find compositor");
				return;
			}

			m_pSurface = wl_compositor_create_surface(m_pCompositor);
			if (m_pSurface == nullptr)
			{
				XENON_LOG_FATAL("Can't create surface");
				return;
			}

			if (m_pShell == nullptr)
			{
				XENON_LOG_FATAL("Haven't got a Wayland shell");
				return;
			}

			m_pShellSurface = wl_shell_get_shell_surface(m_pShell, m_pSurface);
			if (m_pShellSurface == NULL)
			{
				XENON_LOG_FATAL("Can't create shell surface");
				return;
			}

			wl_shell_surface_set_toplevel(m_pShellSurface);
		}

		LinuxWindow::~LinuxWindow()
		{
			wl_display_disconnect(m_pDisplay);
		}

		void LinuxWindow::update()
		{
		}

		bool LinuxWindow::isOpen() const
		{
			return m_IsOpen;
		}
	}
}