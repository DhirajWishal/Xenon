// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../UIComponent.hpp"

#include <vector>

/**
 * Performance metrics class.
 * This UI component contains some performance metrics information like the frame rate graph.
 */
class PerformanceMetrics final : public UIComponent
{
public:
	/**
	 * Default constructor.
	 */
	PerformanceMetrics() = default;

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

private:
	std::vector<float> m_FrameRates = std::vector<float>(1000);
};