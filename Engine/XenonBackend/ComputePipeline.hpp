// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Pipeline.hpp"
#include "Shader.hpp"

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
			 * @param computeShader The compute shader.
			 */
			explicit ComputePipeline(const Device* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, const Shader& computeShader) : Pipeline(pDevice, std::move(pCacheHandler)), m_ComputeShader(computeShader) {}

		protected:
			Shader m_ComputeShader;
		};
	}
}