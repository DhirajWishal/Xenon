// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Common.hpp"

#include <string>

namespace Xenon
{
	/**
	 * Render target type enum.
	 */
	enum class RenderTargetType : uint8_t
	{
		Rasterizer = XENON_BIT_SHIFT(0),
		RayTracer = XENON_BIT_SHIFT(1),
		PathTracer = XENON_BIT_SHIFT(2),

		All = Rasterizer | RayTracer | PathTracer
	};

	XENON_DEFINE_ENUM_OR(RenderTargetType);
	XENON_DEFINE_ENUM_AND(RenderTargetType);

	/**
	 * Instance class.
	 * This is the main class which the user needs to instantiate to use the engine.
	 *
	 * If the requested render target types are not available by the device, it will only enable the render targets which are supported.
	 * A warning will be issued for this issue.
	 */
	class Instance final
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param applicationName The name of the application.
		 * @param applicationVersion The version of the application.
		 * @param renderTargets The render targets which the application will use.
		 */
		explicit Instance(const std::string& applicationName, uint32_t applicationVersion, RenderTargetType renderTargets);

	public:
		/**
		 * Get the application name.
		 *
		 * @return The application name string view.
		 */
		[[nodiscard]] const std::string_view getApplicationName() const { return m_ApplicationName; }

		/**
		 * Get the application version.
		 *
		 * @return The application version.
		 */
		[[nodiscard]] uint32_t getApplicationVersion() const { return m_ApplicationVersion; }

		/**
		 * Get the supported render target types.
		 *
		 * @return The render target types.
		 */
		[[nodsicard]] RenderTargetType getSupportedRenderTargetTypes() const { return m_RenderTargets; }

	private:
		std::string m_ApplicationName;
		uint32_t m_ApplicationVersion;
		RenderTargetType m_RenderTargets;
	};
}