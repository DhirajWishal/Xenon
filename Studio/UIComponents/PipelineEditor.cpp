// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "PipelineEditor.hpp"

#include <imgui.h>
#include <imnodes.h>

namespace /* anonymous */
{
	/**
	 * Set the mini map hovered callback.
	 *
	 * @param nodeID The ID of the node.
	 * @param pUserData The user data associated with the node.
	 */
	void MiniMapHoveredCallback(int32_t nodeID, void* pUserData)
	{
		const auto pNodes = reinterpret_cast<PipeilneEditor*>(pUserData);
		// ImGui::SetTooltip(pNodes[nodeID].getTitle().data());
	}

	constexpr auto DefaultPublicColor = IM_COL32(0, 255, 0, 196);
	constexpr auto DefaultPublicColorHovered = IM_COL32(0, 255, 0, 255);

	constexpr auto DefaultPrivateColor = IM_COL32(255, 0, 0, 196);
	constexpr auto DefaultPrivateColorHovered = IM_COL32(255, 0, 0, 255);

	constexpr auto DefaultProtectedColor = IM_COL32(0, 0, 255, 196);
	constexpr auto DefaultProtectedColorHovered = IM_COL32(0, 0, 255, 255);
}

void PipeilneEditor::begin(std::chrono::nanoseconds delta)
{
	if (m_bIsOpen)
	{
		if (ImGui::Begin("Pipeline Editor", &m_bIsOpen))
		{
			ImNodes::BeginNodeEditor();
			ImNodes::MiniMap();
			ImNodes::EndNodeEditor();
		}

		ImGui::End();
	}
}

void PipeilneEditor::end()
{
}
