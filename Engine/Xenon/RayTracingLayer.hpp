// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Layer.hpp"

#include "../XenonBackend/RayTracer.hpp"

namespace Xenon
{
	/**
	 * Rasterizing layer class.
	 * This class can be used to perform ray tracing and the result will be copied to the next layer's color buffer.
	 */
	class RayTracingLayer : public Layer
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param renderer The renderer reference.
		 * @param priority The priority of the layer.
		 * @param width The width of the render target.
		 * @param height The height of the render target.
		 */
		explicit RayTracingLayer(Renderer& renderer, uint32_t priority, uint32_t width, uint32_t height);

		/**
		 * Get the color attachment from the layer.
		 *
		 * @return The image pointer.
		 */
		XENON_NODISCARD Backend::Image* getColorAttachment() override;

		/**
		 * Get the internally stored ray tracer.
		 *
		 * @return The ray tracer pointer.
		 */
		XENON_NODISCARD Backend::RayTracer* getRayTracer() { return m_pRayTracer.get(); }

		/**
		 * Get the internally stored ray tracer.
		 *
		 * @return The ray tracer pointer.
		 */
		XENON_NODISCARD const Backend::RayTracer* getRayTracer() const { return m_pRayTracer.get(); }

	protected:
		std::unique_ptr<Backend::RayTracer> m_pRayTracer = nullptr;
	};
}