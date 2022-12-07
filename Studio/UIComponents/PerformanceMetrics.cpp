// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "PerformanceMetrics.hpp"

#include <imgui.h>

#include <numeric>

void PerformanceMetrics::begin(std::chrono::nanoseconds delta)
{
	ImGui::Begin("Performance Metrics");

	// Plot the frame rate graph.
	std::ranges::rotate(m_FrameRates, m_FrameRates.end() - 1);
	m_FrameRates[0] = std::nano::den / static_cast<float>(delta.count());
	ImGui::PlotLines("Frame Rate", m_FrameRates.data(), static_cast<int32_t>(m_FrameRates.size()));
	ImGui::Text("Average frame rate: %f", std::accumulate(m_FrameRates.begin(), m_FrameRates.end(), 0.0f) / static_cast<float>(m_FrameRates.size()));
}

void PerformanceMetrics::end()
{
	ImGui::End();
}
