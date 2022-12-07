// Copyright 2022 Dhiraj Wishal
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
};