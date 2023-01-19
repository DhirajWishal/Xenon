// Copyright 2022-2023 Dhiraj Wishal
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
		if (ImGui::Begin("Shader Builder", &m_bIsOpen))
		{
			ImGui::Text("Vertex Shader");
			ImGui::Separator();

			ImNodes::BeginNodeEditor();

			showNodes();

			ImNodes::MiniMap();
			ImNodes::EndNodeEditor();
		}

		ImGui::End();
	}
}

void PipeilneEditor::end()
{
}

void PipeilneEditor::showNodes()
{
	// ImNodes::PushColorStyle(ImNodesCol_TitleBar, m_TitleColor);
	// ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, m_TitleHoveredColor);
	// ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, m_TitleSelectedColor);

	// Show the inputs.
	ImNodes::BeginNode(0);
	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted("Inputs");
	ImNodes::EndNodeTitleBar();

	ImNodes::BeginOutputAttribute(0, ImNodesPinShape_TriangleFilled);
	ImGui::Text("layout (location = 0) in vec2 inPos");
	ImNodes::EndOutputAttribute();

	ImNodes::BeginOutputAttribute(1, ImNodesPinShape_TriangleFilled);
	ImGui::Text("layout (location = 11) in vec2 inUV");
	ImNodes::EndOutputAttribute();

	ImNodes::BeginOutputAttribute(2, ImNodesPinShape_TriangleFilled);
	ImGui::Text("layout (location = 3) in vec4 inColor");
	ImNodes::EndOutputAttribute();
	ImNodes::EndNode();

	// Show the outputs.
	ImNodes::BeginNode(1);
	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted("Outputs");
	ImNodes::EndNodeTitleBar();

	ImNodes::BeginInputAttribute(3, ImNodesPinShape_TriangleFilled);
	ImGui::Text("layout (location = 0) out vec2 outUV");
	ImNodes::EndInputAttribute();

	ImNodes::BeginInputAttribute(4, ImNodesPinShape_TriangleFilled);
	ImGui::Text("layout (location = 0) out vec4 outColor");
	ImNodes::EndInputAttribute();
	ImNodes::EndNode();

	// // Get the IO information.
	// auto& imNodesIO = ImNodes::GetIO();
	// 
	// // Let's render all the input attributes.
	// for (const auto& attribute : m_InputAttributes)
	// {
	// 	// Make sure to push the attribute flag so we can detach the link.
	// 	ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkDetachWithDragClick);
	// 
	// 	// Set the attribute info.
	// 	ImNodes::BeginInputAttribute(attribute.m_AttributeID, ImNodesPinShape_TriangleFilled);
	// 	ImGui::Text(attribute.m_AttributeName.data());
	// 	ImNodes::EndInputAttribute();
	// 
	// 	// Don't forget to pop the attribute flag!
	// 	ImNodes::PopAttributeFlag();
	// }
	// 
	// // Now let's render all the output attributes.
	// int8_t previousProperty = -1;
	// for (const auto& attribute : m_OutputAttributes)
	// {
	// 	// Push the property colors.
	// 	if (attribute.m_Property == -1)
	// 	{
	// 		// Make sure to push the attribute flag so we can detach the link.
	// 		ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkDetachWithDragClick);
	// 
	// 		ImNodes::PushColorStyle(ImNodesCol_Pin, ImNodes::GetStyle().Colors[ImNodesCol_Pin]);
	// 		ImNodes::PushColorStyle(ImNodesCol_PinHovered, ImNodes::GetStyle().Colors[ImNodesCol_PinHovered]);
	// 
	// 		// Set the attribute info.
	// 		ImNodes::BeginOutputAttribute(attribute.m_AttributeID, ImNodesPinShape_TriangleFilled);
	// 		ImGui::Text(attribute.m_AttributeName.data());
	// 		ImNodes::EndOutputAttribute();
	// 
	// 		// Pop the color styles.
	// 		ImNodes::PopColorStyle();
	// 		ImNodes::PopColorStyle();
	// 
	// 		// Don't forget to pop the attribute flag!
	// 		ImNodes::PopAttributeFlag();
	// 	}
	// 	else
	// 	{
	// 		ImNodes::BeginStaticAttribute(attribute.m_AttributeID);
	// 
	// 		if (previousProperty != attribute.m_Property)
	// 		{
	// 			previousProperty = attribute.m_Property;
	// 
	// 			if (attribute.m_Property == 0)
	// 				ImGui::Text("public:");
	// 
	// 			else if (attribute.m_Property == 1)
	// 				ImGui::Text("private:");
	// 
	// 			else if (attribute.m_Property == 2)
	// 				ImGui::Text("protected:");
	// 		}
	// 
	// 		ImGui::Text(("\t" + attribute.m_AttributeName).data());
	// 		ImNodes::EndStaticAttribute();
	// 	}
	// }

	// ImNodes::PopColorStyle();
	// ImNodes::PopColorStyle();
	// ImNodes::PopColorStyle();
}
