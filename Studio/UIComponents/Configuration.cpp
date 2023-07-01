// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Configuration.hpp"

#include "../Layers/ImGuiLayer.hpp"
#include "Xenon/Renderer.hpp"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

void Configuration::begin(std::chrono::nanoseconds delta)
{
	if (m_bIsOpen)
	{
		if (ImGui::Begin("Configuration", &m_bIsOpen))
		{
			ImGui::Text("Camera Position Control");
			ImGui::Separator();

			ImGui::InputFloat3("Position", glm::value_ptr(m_pImGuiLayer->getScene()->getCamera()->m_Position));
			ImGui::InputFloat3("Camera Up", glm::value_ptr(m_pImGuiLayer->getScene()->getCamera()->m_Up));
			ImGui::InputFloat3("Camera Front", glm::value_ptr(m_pImGuiLayer->getScene()->getCamera()->m_Front));
			ImGui::InputFloat3("Camera Right", glm::value_ptr(m_pImGuiLayer->getScene()->getCamera()->m_Right));
			ImGui::InputFloat3("Word Up", glm::value_ptr(m_pImGuiLayer->getScene()->getCamera()->m_WorldUp));

			ImGui::Spacing();
			ImGui::Text("Camera Movement Control");
			ImGui::Separator();

			ImGui::SliderFloat("Movement Bias", &m_pImGuiLayer->getScene()->getCamera()->m_MovementBias, 0.0f, 100.0f);
			ImGui::SliderFloat("Rotation Bias", &m_pImGuiLayer->getScene()->getCamera()->m_RotationBias, 0.0f, 100.0f);
		}

		ImGui::End();
	}
}

void Configuration::end()
{
}
