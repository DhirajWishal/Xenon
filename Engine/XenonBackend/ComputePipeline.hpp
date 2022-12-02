// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Pipeline.hpp"
#include "ShaderSource.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Compute pipeline class.
		 * These types of pipelines contains a single shader and can be used to perform compute operations on the GPU.
		 */
		class ComputePipeline : public Pipeline
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param pCacheHandler The cache handler pointer. This can be null in which case the pipeline creation might get slow.
			 * @param computeShader The compute shader source.
			 */
			explicit ComputePipeline(const Device* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, const ShaderSource& computeShader) : Pipeline(pDevice, std::move(pCacheHandler)), m_ComputeShaderSource(computeShader) {}

		protected:
			ShaderSource m_ComputeShaderSource;
		};
	}
}