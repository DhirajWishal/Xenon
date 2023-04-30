// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>

/**
 * UI Component class.
 * This class is the base class for all the UI components used by the studio.
 */
class UIComponent
{
public:
	/**
	 * Default constructor.
	 */
	UIComponent() = default;

	/**
	 * Begin the component draw.
	 *
	 * @delta The time difference between the previous frame and the current frame in nanoseconds.
	 */
	virtual void begin(std::chrono::nanoseconds delta) = 0;

	/**
	 * End the component draw.
	 */
	virtual void end() = 0;

	/**
	 * Hide the UI.
	 */
	void hide() { m_bIsOpen = false; }

	/**
	 * Show the UI.
	 */
	void show() { m_bIsOpen = true; }

	/**
	 * Check if the component is visible (shown).
	 *
	 * @return True if the component is shown.
	 * @return False if the component is hidden.
	 */
	[[nodiscard]] bool isVisible() const noexcept { return m_bIsOpen; }

protected:
	bool m_bIsOpen = false;
};