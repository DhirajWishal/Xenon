// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "LayerView.hpp"
#include "../Materials/ImGuiMaterial.hpp"

#include "Xenon/Renderer.hpp"

#include <imgui.h>

LayerView::LayerView(ImGuiLayer* pImGuiLayer, Xenon::Layer* pLayer)
	: m_pImGuiLayer(pImGuiLayer)
	, m_pLayer(pLayer)
{
	const auto attachment = pLayer->getColorAttachment();
	m_ImageHash = std::bit_cast<uint64_t>(attachment);
	m_MaterialIdentifier = pImGuiLayer->getRenderer().getInstance().getMaterialDatabase().create<ImGuiMaterial>(m_ImageHash, pImGuiLayer->getRenderer().getInstance(), pLayer->getColorAttachment());
	m_pImGuiLayer->registerMaterial(m_ImageHash, m_MaterialIdentifier);
}

void LayerView::copyLayerImage(Xenon::Backend::CommandRecorder* pCommandRecorder)
{
	static_cast<ImGuiMaterial*>(m_MaterialIdentifier.m_pMaterial)->performCopy(pCommandRecorder, m_pLayer->getColorAttachment());
}

void LayerView::begin(std::chrono::nanoseconds delta)
{
	ImGui::Begin("Layer View");
	ImGui::Image(m_pLayer->getColorAttachment(), ImVec2(static_cast<float>(m_pLayer->getRenderer().getCamera()->getWidth()), static_cast<float>(m_pLayer->getRenderer().getCamera()->getHeight())));
}

void LayerView::end()
{
	ImGui::End();
}
