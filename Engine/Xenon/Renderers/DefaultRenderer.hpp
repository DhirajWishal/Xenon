// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../Renderer.hpp"

namespace Xenon
{
	namespace Experimental
	{
		class GBufferLayer;
		class DirectLightingLayer;
		class LightLUT;

		/**
		 * Default renderer class.
		 * This class performs global illumination and an additional layer can be added for other processing.
		 */
		class DefaultRenderer final : public Renderer
		{
		public:
			/**
			 * Explicit constructor.
			 * This will automatically initialize the object and setup the window.
			 *
			 * @param instance The instance to create the rasterizing renderer with.
			 * @param width The width of the window.
			 * @param height The height of the window.
			 * @param title The title of the renderer window.
			 */
			explicit DefaultRenderer(Instance& instance, uint32_t width, uint32_t height, const std::string& title);

			/**
			 * Set the renderable scene to the layers.
			 *
			 * @param scene The scene reference.
			 */
			void setScene(Scene& scene);

			/**
			 * Get the positive X layer.
			 *
			 * @return The layer pointer.
			 */
			[[nodiscard]] GBufferLayer* getPositiveXLayer() noexcept { return m_pPositiveXLayer; }

			/**
			 * Get the negative X layer.
			 *
			 * @return The layer pointer.
			 */
			[[nodiscard]] GBufferLayer* getNegativeXLayer() noexcept { return m_pNegativeXLayer; }

			/**
			 * Get the positive Y layer.
			 *
			 * @return The layer pointer.
			 */
			[[nodiscard]] GBufferLayer* getPositiveYLayer() noexcept { return m_pPositiveYLayer; }

			/**
			 * Get the negative Y layer.
			 *
			 * @return The layer pointer.
			 */
			[[nodiscard]] GBufferLayer* getNegativeYLayer() noexcept { return m_pNegativeYLayer; }

			/**
			 * Get the positive Z layer.
			 *
			 * @return The layer pointer.
			 */
			[[nodiscard]] GBufferLayer* getPositiveZLayer() noexcept { return m_pPositiveZLayer; }

			/**
			 * Get the negative Z layer.
			 *
			 * @return The layer pointer.
			 */
			[[nodiscard]] GBufferLayer* getNegativeZLayer() noexcept { return m_pNegativeZLayer; }

			/**
			 * Get the direct lighting layer.
			 *
			 * @return The layer pointer.
			 */
			[[nodiscard]] DirectLightingLayer* getDirectLightingLayer() noexcept { return m_pDirectLightingLayer; }

		private:
			GBufferLayer* m_pPositiveXLayer = nullptr;
			GBufferLayer* m_pNegativeXLayer = nullptr;
			GBufferLayer* m_pPositiveYLayer = nullptr;
			GBufferLayer* m_pNegativeYLayer = nullptr;
			GBufferLayer* m_pPositiveZLayer = nullptr;
			GBufferLayer* m_pNegativeZLayer = nullptr;

			DirectLightingLayer* m_pDirectLightingLayer = nullptr;

			LightLUT* m_pLightLUT = nullptr;
		};
	}
}