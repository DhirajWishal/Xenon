// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Buffer.hpp"

#include <vector>

namespace Xenon
{
	namespace Backend
	{
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
			explicit Camera(uint32_t width, uint32_t height) : m_Width(width), m_Height(height) {}

			/**
			 * Default virtual destructor.
			 */
			virtual ~Camera() = default;

			/**
			 * Get the view ports.
			 * Each view port will result in a single pass using the provided information.
			 *
			 * @return The view ports of the camera.
			 */
			[[nodiscard]] virtual std::vector<Viewport> getViewports() = 0;

		public:
			/**
			 * Get the width of the camera frame.
			 *
			 * @return The width.
			 */
			[[nodiscard]] uint32_t getWidth() const { return m_Width; }

			/**
			 * Get the height of the camera frame.
			 *
			 * @return The height.
			 */
			[[nodiscard]] uint32_t getHeight() const { return m_Height; }

		public:
			uint32_t m_Width = 0;
			uint32_t m_Height = 0;
		};
	}
}