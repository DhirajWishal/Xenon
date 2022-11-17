// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Pipeline.hpp"
#include "Rasterizer.hpp"
#include "ShaderSource.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Rasterizing pipeline specification.
		 */
		struct RasterizingPipelineSpecification final
		{
			ShaderSource m_VertexShader;
			ShaderSource m_FragmentShader;	// AKA the pixel shader.
		};

		/**
		 * Rasterizing pipeline class.
		 * This class is used by a rasterizer to perform rasterization to a set of primitives.
		 */
		class RasterizingPipeline : public Pipeline
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param pCacheHandler The cache handler pointer.
			 * @param pRasterizer The rasterizer pointer.
			 * @param specification The pipeline specification.
			 */
			explicit RasterizingPipeline(Device* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, [[maybe_unused]] Rasterizer* pRasterizer, const RasterizingPipelineSpecification& specification)
				: Pipeline(pDevice, std::move(pCacheHandler)), m_Specification(specification) {}

		private:
			RasterizingPipelineSpecification m_Specification;
		};
	}
}