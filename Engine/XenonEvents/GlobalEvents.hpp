// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Mouse.hpp"
#include "Keyboard.hpp"

namespace Xenon
{
	/**
	 * Global events class.
	 */
	class GlobalEvents final
	{
	public:
		/**
		 * Default constructor.
		 */
		GlobalEvents() = default;

		/**
		 * Get the global static instance.
		 *
		 * @return The object instance.
		 */
		[[nodiscard]] static GlobalEvents& Get();

	public:
		Mouse m_Mouse;
		Keyboard m_Keyboard;
	};
}