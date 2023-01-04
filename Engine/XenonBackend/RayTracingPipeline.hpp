// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Pipeline.hpp"
#include "Shader.hpp"
#include "ShaderBindingTable.hpp"

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

		/**
		 * Shader group structure.
		 */
		struct ShaderGroup final
		{
			Shader m_RayGenShader = {};
			Shader m_IntersectionShader = {};
			Shader m_AnyHitShader = {};
			Shader m_ClosestHitShader = {};
			Shader m_MissShader = {};
			Shader m_CallableShader = {};
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
		 * Ray tracing pipeline specification structure.
		 */
		struct RayTracingPipelineSpecification final
		{
			std::vector<ShaderGroup> m_ShaderGroups;

			uint32_t m_MaxPayloadSize = 0;
			uint32_t m_MaxAttributeSize = 0;

			uint32_t m_MaxRayRecursionDepth = 4;
		};

		/**
		 * Shader group argument structure.
		 * This contains all the required shader group arguments. Make sure that the shader group count is the same as the provided shader group for the ray tracing pipeline.
		 */
		struct ShaderGroupArgument final
		{
			Descriptor* m_pUserDefinedDescriptor = nullptr;
			Descriptor* m_pMaterialDescriptor = nullptr;
			Descriptor* m_pSceneDescriptor = nullptr;
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
			 * @param specification The pipeline specification.
			 */
			explicit RayTracingPipeline(const Device* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, const RayTracingPipelineSpecification& specification)
				: Pipeline(pDevice, std::move(pCacheHandler))
				, m_Specification(specification) {}

			/**
			 * Create a new ray generation shader binding table.
			 *
			 * @param bindingGroups The binding groups.
			 * @return The created shader binding table.
			 */
			[[nodiscard]] virtual std::unique_ptr<ShaderBindingTable> createShaderBindingTable(const std::vector<BindingGroup>& bindingGroups) = 0;

			/**
			 * Get the maximum possible ray recursion depth of the pipeline.
			 *
			 * @return The depth.
			 */
			[[nodiscard]] uint32_t getMaximumRayRecursionDepth() const noexcept { return m_MaximumRayRecursionDepth; }

			/**
			 * Get the pipeline specification.
			 *
			 * @return The specification.
			 */
			[[nodiscard]] const RayTracingPipelineSpecification& getSpecification() const noexcept { return m_Specification; }

		protected:
			std::vector<ShaderGroup> m_ShaderGroups = {};
			uint32_t m_MaximumRayRecursionDepth = 0;
			RayTracingPipelineSpecification m_Specification = {};
		};
	}
}
