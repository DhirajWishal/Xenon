// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "LayerView.hpp"
#include "../Layers/ImGuiLayer.hpp"

#include "Xenon/Renderer.hpp"

#include <imgui.h>

LayerView::LayerView(ImGuiLayer* pImGuiLayer)
	: m_pImGuiLayer(pImGuiLayer)
{
	// Create the image sampler.
	Xenon::Backend::ImageSamplerSpecification imageSamplerSpecification = {};
	imageSamplerSpecification.m_AddressModeU = Xenon::Backend::AddressMode::ClampToEdge;
	imageSamplerSpecification.m_AddressModeV = Xenon::Backend::AddressMode::ClampToEdge;
	imageSamplerSpecification.m_AddressModeW = Xenon::Backend::AddressMode::ClampToEdge;
	imageSamplerSpecification.m_BorderColor = Xenon::Backend::BorderColor::OpaqueWhiteFLOAT;
	imageSamplerSpecification.m_MaxLevelOfDetail = 1.0f;
	m_pSampler = m_pImGuiLayer->getRenderer().getInstance().getFactory()->createImageSampler(m_pImGuiLayer->getRenderer().getInstance().getBackendDevice(), imageSamplerSpecification);
}

void LayerView::setLayer(Xenon::Layer* pLayer)
{
	m_pLayerToShow = pLayer;

	const auto pImage = pLayer->getColorAttachment();

	// Create the image.
	Xenon::Backend::ImageSpecification imageSpecification = pImage->getSpecification();
	imageSpecification.m_Usage = Xenon::Backend::ImageUsage::Graphics;
	m_pImage = m_pImGuiLayer->getRenderer().getInstance().getFactory()->createImage(m_pImGuiLayer->getRenderer().getInstance().getBackendDevice(), imageSpecification);

	// Create the image view.
	m_pImageView = m_pImGuiLayer->getRenderer().getInstance().getFactory()->createImageView(m_pImGuiLayer->getRenderer().getInstance().getBackendDevice(), m_pImage.get(), {});

	m_ImageHash = m_pImGuiLayer->getImageID(m_pImage.get(), m_pImageView.get(), m_pSampler.get());
}

void LayerView::copyLayerImage(Xenon::Backend::CommandRecorder* pCommandRecorder)
{
	m_pImage->copyFrom(m_pLayerToShow->getColorAttachment(), pCommandRecorder);
}

void LayerView::begin(std::chrono::nanoseconds delta)
{
	if (m_bIsOpen)
	{
		if (ImGui::Begin("Layer View", &m_bIsOpen))
		{
			const auto size = ImGui::GetContentRegionAvail();
			m_Size.x = size.x;
			m_Size.y = size.y;

			ImGui::Image(std::bit_cast<void*>(m_ImageHash), size);
			m_bIsInFocus = ImGui::IsWindowFocused();

			const auto pos = ImGui::GetWindowPos();
			m_Position.x = pos.x;
			m_Position.y = pos.y;
		}

		ImGui::End();
	}
}

void LayerView::end()
{
}
