// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../UIComponent.hpp"

#include <functional>

/**
 * PopUp type enum.
 */
enum class PopUpType : uint8_t
{
	// Here the pop-up will have it's own widget on top of the other widgets but will not block the other UI elements.
	PopUp,

	// Here the pop-up will have it's own widget on top of the other widgets but will block the other UI elements.
	Overlay
};

/**
 * PopUp option structure.
 * This contains information about a single option of the pop-up and how to handle it.
 */
struct PopUpOption final
{
	PopUpOption() = default;
	explicit PopUpOption(std::string_view string, const std::function<void()>& handler) : m_String(string), m_Handler(handler) {}

	std::string_view m_String;
	std::function<void()> m_Handler;
};

/**
 * Pop-up class.
 * This is used to inform the user about certain things happening in the studio.
 */
class PopUp final : public UIComponent
{
public:
	/**
	 * Explicit constructor.
	 *
	 * @param title The title of the pop-up.
	 * @param description The description of the pop-up.
	 * @param options The button options to exit the pop-up. None means no buttons will be displayed.
	 * @param type The type of the pop-up. Default is pop-up.
	 */
	explicit PopUp(std::string_view title, std::string_view description, const std::vector<PopUpOption>& options, PopUpType type = PopUpType::PopUp);

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
	std::string_view m_Title;
	std::string_view m_Description;

	std::vector<PopUpOption> m_Options;

	PopUpType m_Type = PopUpType::PopUp;
};
