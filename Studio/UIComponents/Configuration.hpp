// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../UIComponent.hpp"

class ImGuiLayer;

/**
 * Configuration class.
 * This component can update some simple configurations like the renderer's camera.
 */
class Configuration final : public UIComponent
{
public:
	/**
	 * Explicit constructor.
	 *
	 * @param pImGuiLayer The ImGui layer pointer.
	 * @param pLayer The layer to be shown.
	 */
	explicit Configuration(ImGuiLayer* pImGuiLayer) : m_pImGuiLayer(pImGuiLayer) {}

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
	ImGuiLayer* m_pImGuiLayer = nullptr;
};