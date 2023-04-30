// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#include "LayerView.hpp"
#include "../Layers/ImGuiLayer.hpp"
#include "../StudioConfiguration.hpp"

#include "Xenon/Renderer.hpp"
#include "Xenon/MonoCamera.hpp"

#include <imgui.h>
#include <ImGuizmo.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <numbers>

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
	// Wait idle while the previous commands are done.
	m_pImGuiLayer->getRenderer().getInstance().getBackendDevice()->waitIdle();

	// Setup the layer info.
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
	if (m_pLayerToShow && m_pImage)
		m_pImage->copyFrom(m_pLayerToShow->getColorAttachment(), pCommandRecorder);
}

void LayerView::begin(std::chrono::nanoseconds delta)
{
	if (m_bIsOpen)
	{
		if (ImGui::Begin("Layer View", &m_bIsOpen))
		{
			// Show the available options.
			showOptionsCombo();

			// Setup the region sizes.
			const auto size = ImGui::GetContentRegionAvail();
			m_Size.x = size.x;
			m_Size.y = size.y;

			const auto pos = ImGui::GetWindowPos();
			m_Position.x = pos.x;
			m_Position.y = pos.y;

			// Check if the window is in focus.
			m_bIsInFocus = ImGui::IsWindowFocused();

			// Show the image.
			ImGui::Image(std::bit_cast<void*>(m_ImageHash), size);
		}

		// Finally show the ImGuizmo stuff.
		const auto pCamera = m_pImGuiLayer->getScene()->getCamera()->as<Xenon::MonoCamera>();
		auto [view, projection] = pCamera->getCameraBuffer();

		const auto position = getPosition();
		const auto size = getSize();

		view[0][1] = -view[0][1];
		view[1][1] = -view[1][1];
		view[2][1] = -view[2][1];

#ifdef XENON_PLATFORM_WINDOWS
		// Flip if we're using Vulkan (because of the inverted y-axis in DirectX.
		view[3][1] = StudioConfiguration::GetInstance().getCurrentBackendType() == Xenon::BackendType::Vulkan ? -view[3][1] : view[3][1];

#else
		view[3][1] = -view[3][1];

#endif // XENON_PLATFORM_WINDOWS

		ImGuizmo::SetDrawlist();
		constexpr auto currentGizmoMode = ImGuizmo::LOCAL;
		constexpr auto currentGizmoOperation = ImGuizmo::UNIVERSAL;

		auto lock = std::scoped_lock(m_pImGuiLayer->getScene()->getMutex());
		for (const auto group : m_pImGuiLayer->getScene()->getRegistry().view<Xenon::Components::LightSource>())
		{
			// Get the transform and compute the model matrix.
			glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), m_pImGuiLayer->getScene()->getRegistry().get<Xenon::Components::LightSource>(group).m_Position);

			// Setup and show ImGuizmo.
			ImGuizmo::SetRect(position.x, position.y, size.x, size.y);
			ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), currentGizmoOperation, currentGizmoMode, glm::value_ptr(modelMatrix), nullptr, nullptr, nullptr, nullptr);

			// Decompose the matrix and update the component.
			glm::vec3 scale;
			glm::quat rotation;
			glm::vec3 translation;
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::decompose(modelMatrix, scale, rotation, translation, skew, perspective);

			const auto patchFunction = [translation, rotation](auto& object) 
			{
				object.m_Position = translation; 
				object.m_Direction = glm::eulerAngles(rotation) * std::numbers::pi_v<float> / 180.f; 
			};

			m_pImGuiLayer->getScene()->getRegistry().patch<Xenon::Components::LightSource>(group, patchFunction);
		}

		ImGui::End();
	}
}

void LayerView::end()
{
}

void LayerView::addLayerOption(const std::string& title, Xenon::Layer* pLayer)
{
	m_LayerOptions.emplace_back(title, pLayer);
}

void LayerView::showOptionsCombo()
{
	if (ImGui::BeginCombo("Select Layer", m_SelectedOption.first.c_str()))
	{
		for (const auto& option : m_LayerOptions)
		{
			if (ImGui::Selectable(option.first.c_str(), m_SelectedOption.first == option.first))
			{
				m_SelectedOption = option;
				setLayer(option.second);
			}
		}

		ImGui::EndCombo();
	}
}
