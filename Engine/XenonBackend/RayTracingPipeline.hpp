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
		 * Shader record structure.
		 * This is a collection of similar typed shaders.
		 */
		struct ShaderRecord final
		{
			std::vector<ShaderSource> m_Shaders;
		};

		struct ShaderGroup final
		{
			ShaderSource m_RayGenShader = {};
			ShaderSource m_IntersectionShader = {};
			ShaderSource m_AnyHitShader = {};
			ShaderSource m_ClosestHitShader = {};
			ShaderSource m_MissShader = {};
			ShaderSource m_CallableShader = {};
		};

		/**
		 * Shader table structure.
		 */
		struct ShaderTable final
		{
			ShaderRecord m_RayGenRecord;
			ShaderRecord m_IntersectionRecord;
			ShaderRecord m_AnyHitRecord;
			ShaderRecord m_ClosestHitRecord;
			ShaderRecord m_MissRecord;
			ShaderRecord m_CallableRecord;

			std::vector<ShaderGroup> m_ShaderGroups;
		};

		/**
		 * Ray tracing pipeline class.
		 * This pipeline is used to perform ray tracing on objects.
		 */
		class RayTracingPipeline : public Pipeline
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param pCacheHandler The cache handler pointer. This can be null in which case the pipeline creation might get slow.
			 * @param shaderGroups The shader groups.
			 * @param maxRayRecursion The maximum ray recursion depth. Default is 4.
			 */
			explicit RayTracingPipeline(const Device* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, const std::vector<ShaderGroup>& shaderGroups, uint32_t maxRayRecursion = 4)
				: Pipeline(pDevice, std::move(pCacheHandler))
				, m_ShaderGroups(shaderGroups)
				, m_MaximumRayRecursionDepth(maxRayRecursion) {}

			/**
			 * Get the maximum possible ray recursion depth of the pipeline.
			 *
			 * @return The depth.
			 */
			[[nodiscard]] uint32_t getMaximumRayRecursionDepth() const noexcept { return m_MaximumRayRecursionDepth; }

		protected:
			std::vector<ShaderGroup> m_ShaderGroups = {};
			uint32_t m_MaximumRayRecursionDepth = 0;
		};
	}
}
