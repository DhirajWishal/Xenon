// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../UIComponent.hpp"
#include "../Layers/ImGuiLayer.hpp"

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
	explicit LayerView(ImGuiLayer* pImGuiLayer, Xenon::Layer* pLayer);

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

private:
	ImGuiLayer* m_pImGuiLayer = nullptr;
	Xenon::Layer* m_pLayer = nullptr;

	uint64_t m_ImageHash = 0;
	Xenon::MaterialIdentifier m_MaterialIdentifier;
};