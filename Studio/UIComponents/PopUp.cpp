// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "PopUp.hpp"

#include <imgui.h>

PopUp::PopUp(std::string_view title, std::string_view description, const std::vector<PopUpOption>& options, PopUpType type /*= PopUpType::PopUp*/)
	: m_Title(title), m_Description(description), m_Options(options), m_Type(type)
{
}

void PopUp::begin(std::chrono::nanoseconds delta)
{
	const auto result = m_Type == PopUpType::PopUp ? ImGui::BeginPopup(m_Title.data()) : ImGui::BeginPopupModal(m_Title.data());

	if (result)
	{
		ImGui::Text(m_Description.data());

		for (const auto& option : m_Options)
		{
			if (ImGui::Button(option.m_String.data()))
				option.m_Handler();
		}

		ImGui::EndPopup();
	}
}

void PopUp::end()
{
}
