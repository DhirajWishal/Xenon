// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../UIComponent.hpp"

/**
 * Pipeline editor class.
 * This contains a node graph and other information about a pipeline which can be edited.
 */
class PipeilneEditor final : public UIComponent
{
public:
	/**
	 * Default constructor.
	 */
	PipeilneEditor() = default;

	/**
	 * Begin the component draw.
	 *
	 * @delta The time difference between the previous frame and the current frame in nanoseconds.
	 */
	void begin(std::chrono::nanoseconds delta) override;

	/**
	 * End the component draw.
	 */
	void end() override;
};