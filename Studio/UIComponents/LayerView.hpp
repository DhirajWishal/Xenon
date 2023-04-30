// Copyright 2022-2023 Nexonous
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
	using LayerOption = std::pair<std::string, Xenon::Layer*>;

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

	/**
	 * Get the window position.
	 *
	 * @return The position.
	 */
	[[nodiscard]] glm::vec2 getPosition() const noexcept { return m_Position; }

	/**
	 * Get the window size.
	 *
	 * @return The size.
	 */
	[[nodiscard]] glm::vec2 getSize() const noexcept { return m_Size; }

	/**
	 * Add an option to the layer view.
	 *
	 * @param title The layer's title.
	 * @param pLayer The layer pointer.
	 */
	void addLayerOption(const std::string& title, Xenon::Layer* pLayer);

private:
	/**
	 * Show the options combo.
	 */
	void showOptionsCombo();

private:
	ImGuiLayer* m_pImGuiLayer = nullptr;
	Xenon::Layer* m_pLayerToShow = nullptr;
	LayerOption m_SelectedOption;

	uintptr_t m_ImageHash = 0;

	std::unique_ptr<Xenon::Backend::Image> m_pImage = nullptr;
	std::unique_ptr<Xenon::Backend::ImageView> m_pImageView = nullptr;
	std::unique_ptr<Xenon::Backend::ImageSampler> m_pSampler = nullptr;

	std::vector<LayerOption> m_LayerOptions;

	glm::vec2 m_Position;
	glm::vec2 m_Size;

	bool m_bIsInFocus = false;
};