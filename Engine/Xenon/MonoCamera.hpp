// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.hpp"
#include "../XenonBackend/Camera.hpp"

#include <glm/mat4x4.hpp>

namespace Xenon
{
	/**
	 * Mono camera class.
	 * This is a simple camera with one viewport.
	 */
	class MonoCamera final : public Backend::Camera
	{
		/**
		 * Camera buffer structure.
		 * This object contains the camera's view and projection matrices.
		 */
		struct CameraBuffer final
		{
			glm::mat4 m_View = glm::mat4(1.0f);
			glm::mat4 m_Projection = glm::mat4(1.0f);
		};

		using Clock = std::chrono::steady_clock;
		using TimePoint = Clock::time_point;

	public:
		/**
		 * Explicit constructor.
		 *
		 * @param instance The instance to which the camera is bound to.
		 * @param width The width of the camera frame.
		 * @param height The height of the camera frame.
		 */
		explicit MonoCamera(Instance& instance, uint32_t width, uint32_t height);

		/**
		 * Update the camera.
		 */
		void update() override;

		/**
		 * Get the view ports.
		 * Each view port will result in a single pass using the provided information.
		 *
		 * @return The view ports of the camera.
		 */
		XENON_NODISCARD std::vector<Backend::Viewport> getViewports() override { return { m_Viewport }; }

		/**
		 * Get the camera buffer with the camera's matrices.
		 *
		 * @return The camera buffer.
		 */
		XENON_NODISCARD const CameraBuffer& getCameraBuffer() const noexcept { return m_CameraBuffer; }

	private:
		BackendType m_BackendType = BackendType::Any;
		CameraBuffer m_CameraBuffer;
		Backend::Viewport m_Viewport;

		std::unique_ptr<Backend::Buffer> m_pUniformBuffer = nullptr;
	};
}