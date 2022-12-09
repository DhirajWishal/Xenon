// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../UIComponent.hpp"

#include "Xenon/Layer.hpp"

class ImGuiLayer;

/**
 * Layer view class.
 * This component can be used to view a layer's color.
 */
class LayerView final : public UIComponent
{
public:
	/**
	 * Explicit constructor.
	 *
	 * @param pImGuiLayer The ImGui layer pointer.
	 * @param pLayer The layer to be shown.
	 */
	explicit LayerView(ImGuiLayer* pImGuiLayer);

	/**
	 * Set the layer to be shown.
	 *
	 * @param pLayer The layer to show.
	 */
	void setLayer(Xenon::Layer* pLayer);

	/**
	 * Copy the layer's color image to the material image.
	 *
	 * @param pCommandRecorder The command recorder pointer.
	 */
	void copyLayerImage(Xenon::Backend::CommandRecorder* pCommandRecorder);

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

	/**
	 * Check if the layer is in focus.
	 *
	 * @return True if the layer is in focus.
	 * @return False if the layer is not in focus.
	 */
	[[nodiscard]] bool isInFocus() const noexcept { return m_bIsInFocus; }

private:
	ImGuiLayer* m_pImGuiLayer = nullptr;
	Xenon::Layer* m_pLayerToShow = nullptr;

	uint64_t m_ImageHash = 0;
	Xenon::MaterialIdentifier m_MaterialIdentifier;

	bool m_bIsInFocus = false;
};