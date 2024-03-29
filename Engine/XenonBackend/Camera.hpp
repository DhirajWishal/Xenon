// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Buffer.hpp"

#include <glm/vec3.hpp>

namespace Xenon
{
	namespace Backend
	{
		class Descriptor;

		/**
		 * Viewport structure.
		 * This contains information about a single viewport and it's information which is used when rendering.
		 */
		struct Viewport final
		{
			Buffer* m_pUniformBuffer = nullptr;	// This contains the view and projection matrices.

			float m_XOffset = 0;
			float m_YOffset = 0;

			float m_Width = 0;
			float m_Height = 0;

			float m_MinDepth = 0;
			float m_MaxDepth = 1;
		};

		/**
		 * Xenon camera class.
		 * This is the base class for all the cameras which are to be used with the engine.
		 */
		class Camera : public BackendObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param width The width of the camera frame.
			 * @param height The height of the camera frame.
			 */
			explicit Camera(uint32_t width, uint32_t height) : m_AspectRatio(static_cast<float>(width) / static_cast<float>(height)), m_Width(width), m_Height(height) {}

			/**
			 * Update the camera.
			 */
			virtual void update() = 0;

			/**
			 * Get the view ports.
			 * Each view port will result in a single pass using the provided information.
			 *
			 * @return The view ports of the camera.
			 */
			XENON_NODISCARD virtual std::vector<Viewport> getViewports() = 0;

			/**
			 * Move the camera forward.
			 *
			 * @param delta The time delta in nanoseconds.
			 */
			void moveForward(std::chrono::nanoseconds delta) { m_Position += m_Front * (static_cast<float>(delta.count()) / std::nano::den) * m_MovementBias; }

			/**
			 * Move the camera backward.
			 *
			 * @param delta The time delta in nanoseconds.
			 */
			void moveBackward(std::chrono::nanoseconds delta) { m_Position -= m_Front * (static_cast<float>(delta.count()) / std::nano::den) * m_MovementBias; }

			/**
			 * Move the camera to the left.
			 *
			 * @param delta The time delta in nanoseconds.
			 */
			void moveLeft(std::chrono::nanoseconds delta) { m_Position -= m_Right * (static_cast<float>(delta.count()) / std::nano::den) * m_MovementBias; }

			/**
			 * Move the camera to the right.
			 *
			 * @param delta The time delta in nanoseconds.
			 */
			void moveRight(std::chrono::nanoseconds delta) { m_Position += m_Right * (static_cast<float>(delta.count()) / std::nano::den) * m_MovementBias; }

			/**
			 * Move the camera up.
			 *
			 * @param delta The time delta in nanoseconds.
			 */
			void moveUp(std::chrono::nanoseconds delta) { m_Position += m_Up * (static_cast<float>(delta.count()) / std::nano::den) * m_MovementBias; }

			/**
			 * Move the camera down.
			 *
			 * @param delta The time delta in nanoseconds.
			 */
			void moveDown(std::chrono::nanoseconds delta) { m_Position -= m_Up * (static_cast<float>(delta.count()) / std::nano::den) * m_MovementBias; }

			/**
			 * Rotate the camera up.
			 *
			 * @param delta The time delta in nanoseconds.
			 */
			void rotateUp(std::chrono::nanoseconds delta) { m_Pitch += (static_cast<float>(delta.count()) / std::nano::den) * m_RotationBias; }

			/**
			 * Rotate the camera down.
			 *
			 * @param delta The time delta in nanoseconds.
			 */
			void rotateDown(std::chrono::nanoseconds delta) { m_Pitch -= (static_cast<float>(delta.count()) / std::nano::den) * m_RotationBias; }

			/**
			 * Rotate the camera to the left.
			 *
			 * @param delta The time delta in nanoseconds.
			 */
			void rotateLeft(std::chrono::nanoseconds delta) { m_Yaw += (static_cast<float>(delta.count()) / std::nano::den) * m_RotationBias; }

			/**
			 * Rotate the camera to the right.
			 *
			 * @param delta The time delta in nanoseconds.
			 */
			void rotateRight(std::chrono::nanoseconds delta) { m_Yaw -= (static_cast<float>(delta.count()) / std::nano::den) * m_RotationBias; }

			/**
			 * Update the Yaw of the camera.
			 * This will just increment the yaw value by the incoming value.
			 *
			 * @param offset The offset to set.
			 * @param delta The delta time.
			 */
			void updateYaw(float offset, std::chrono::nanoseconds delta) { m_Yaw += (static_cast<float>(delta.count()) / std::nano::den) * m_RotationBias * offset; }

			/**
			 * Update the Pitch of the camera.
			 * This will just increment the [yaw]itch value by the incoming value.
			 * It will also clamp the pitch value to stay within -89 - 89.
			 *
			 * @param offset The offset to set.
			 * @param delta The delta time.
			 */
			void updatePitch(float offset, std::chrono::nanoseconds delta)
			{
				m_Pitch += (static_cast<float>(delta.count()) / std::nano::den) * m_RotationBias * offset;

				if (m_Pitch > 89.0f) m_Pitch = 89.0f;
				if (m_Pitch < -89.0f) m_Pitch = -89.0f;
			}

		public:
			/**
			 * Get the width of the camera frame.
			 *
			 * @return The width.
			 */
			XENON_NODISCARD uint32_t getWidth() const noexcept { return m_Width; }

			/**
			 * Get the height of the camera frame.
			 *
			 * @return The height.
			 */
			XENON_NODISCARD uint32_t getHeight() const noexcept { return m_Height; }

		public:
			glm::vec3 m_Position = glm::vec3(0.0f, 1.0f, 0.0f);
			glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
			glm::vec3 m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
			glm::vec3 m_Right = glm::vec3(1.0f, 0.0f, 0.0f);
			glm::vec3 m_WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

			float m_MovementBias = 1.0f;
			float m_RotationBias = 1.0f;	// Radians per second.

			float m_FieldOfView = 60.0f;
			float m_AspectRatio = 0.0f;
			float m_FarPlane = 256.0f;
			float m_NearPlane = 0.001f;

			float m_Yaw = 90.0f;
			float m_Pitch = 0.0f;

			uint32_t m_Width = 0;
			uint32_t m_Height = 0;
		};
	}
}