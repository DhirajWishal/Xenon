// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Xenon/Instance.hpp"
#include "Xenon/Renderer.hpp"
#include "Xenon/Scene.hpp"

/**
 * Studio class.
 */
class Studio final
{
	/**
	 * Light bulb structure.
	 * This contains all the necessary information about a single light source.
	 */
	struct LightBulb final
	{
		std::unique_ptr<Xenon::Backend::Image> m_pImage = nullptr;
		std::unique_ptr<Xenon::Backend::ImageView> m_pImageView = nullptr;
		std::unique_ptr<Xenon::Backend::ImageSampler> m_pImageSampler = nullptr;
	};

public:
	/**
	 * Explicit constructor.
	 *
	 * @param type The type of the backend. Default is any.
	 */
	explicit Studio(Xenon::BackendType type = Xenon::BackendType::Any);

	/**
	 * Run the studio.
	 */
	void run();

private:
	/**
	 * Update the camera.
	 *
	 * @param delta The delta time.
	 */
	void updateCamera(std::chrono::nanoseconds delta);

	/**
	 * Create a new light source.
	 *
	 * @return The light source group.
	 */
	[[nodiscard]] Xenon::Group createLightSource();

	/**
	 * Update the light source.
	 *
	 * @param group The light source group.
	 */
	void updateLightSource(Xenon::Group group);

private:
	Xenon::Instance m_Instance;
	Xenon::Scene m_Scene;
	Xenon::Renderer m_Renderer;

	float m_LastX = 0.0f;
	float m_LastY = 0.0f;

	bool m_bFirstMouse = true;
};