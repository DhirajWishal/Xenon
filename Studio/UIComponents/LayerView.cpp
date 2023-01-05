// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "LayerView.hpp"
#include "../Materials/ImGuiMaterial.hpp"
#include "../Layers/ImGuiLayer.hpp"

#include "Xenon/Renderer.hpp"

#include <imgui.h>

LayerView::LayerView(ImGuiLayer* pImGuiLayer)
	: m_pImGuiLayer(pImGuiLayer)
{
}

void LayerView::setLayer(Xenon::Layer* pLayer)
{
	m_pLayerToShow = pLayer;

	const auto attachment = pLayer->getColorAttachment();
	const auto hash = std::bit_cast<uint64_t>(attachment);

	if (m_ImageHash != hash)
	{
		m_ImageHash = hash;
		m_MaterialIdentifier = m_pImGuiLayer->getRenderer().getInstance().getMaterialDatabase().create<ImGuiMaterial>(m_ImageHash, m_pImGuiLayer->getRenderer().getInstance(), attachment);
		m_pImGuiLayer->registerMaterial(m_ImageHash, m_MaterialIdentifier);
	}
}

void LayerView::copyLayerImage(Xenon::Backend::CommandRecorder* pCommandRecorder)
{
	static_cast<ImGuiMaterial*>(m_MaterialIdentifier.m_pMaterial)->performCopy(pCommandRecorder, m_pLayerToShow->getColorAttachment());
}

void LayerView::begin(std::chrono::nanoseconds delta)
{
	if (m_bIsOpen)
	{
		if (ImGui::Begin("Layer View", &m_bIsOpen))
		{
			ImGui::Image(std::bit_cast<void*>(m_ImageHash), ImGui::GetContentRegionAvail());
			m_bIsInFocus = ImGui::IsWindowFocused();
		}

		ImGui::End();
	}
}

void LayerView::end()
{
}
