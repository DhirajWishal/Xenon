// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Xenon/Instance.hpp"
#include "Xenon/MonoCamera.hpp"
#include "Xenon/Renderer.hpp"

/**
 * Studio class.
 */
class Studio final
{
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

private:
	Xenon::Instance m_Instance;
	Xenon::MonoCamera m_Camera;
	Xenon::Renderer m_Renderer;

	float m_LastX = 0.0f;
	float m_LastY = 0.0f;

	bool m_bFirstMouse = true;
};