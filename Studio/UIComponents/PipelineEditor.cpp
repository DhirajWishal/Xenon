// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "PipelineEditor.hpp"

#include <imgui.h>
#include <imnodes.h>

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
