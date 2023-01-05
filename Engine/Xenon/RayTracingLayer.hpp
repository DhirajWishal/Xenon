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
		 * @param pCamera The camera which is used to render the scene.
		 */
		explicit RayTracingLayer(Renderer& renderer, Backend::Camera* pCamera);

		/**
		 * Get the color attachment from the layer.
		 *
		 * @return The image pointer.
		 */
		[[nodiscard]] Backend::Image* getColorAttachment() override;

		/**
		 * Get the internally stored ray tracer.
		 *
		 * @return The ray tracer pointer.
		 */
		[[nodsicard]] Backend::RayTracer* getRayTracer() { return m_pRayTracer.get(); }

		/**
		 * Get the internally stored ray tracer.
		 *
		 * @return The ray tracer pointer.
		 */
		[[nodsicard]] const Backend::RayTracer* getRayTracer() const { return m_pRayTracer.get(); }

	protected:
		std::unique_ptr<Backend::RayTracer> m_pRayTracer = nullptr;
	};
}