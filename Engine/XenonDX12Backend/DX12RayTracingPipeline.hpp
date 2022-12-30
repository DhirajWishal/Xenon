// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/RayTracingPipeline.hpp"

#include "DX12DescriptorHeapManager.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * DirectX 12 ray tracing pipeline class.
		 */
		class DX12RayTracingPipeline final : public RayTracingPipeline, public DX12DescriptorHeapManager
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
			explicit DX12RayTracingPipeline(DX12Device* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, const std::vector<ShaderGroup>& shaderGroups, uint32_t maxRayRecursion = 4);

			/**
			 * Destructor.
			 */
			~DX12RayTracingPipeline() override = default;

			/**
			 * Create a new descriptor.
			 *
			 * @param type The descriptor type to create.
			 * @return The descriptor pointer. It will return nullptr if the descriptor type is not present in the pipeline.
			 */
			[[nodiscard]] std::unique_ptr<Descriptor> createDescriptor(DescriptorType type) override;

		private:
			/**
			 * Create the DXIL library.
			 *
			 * @param stateObject The state object to create the sub-object from.
			 * @param pShaderBlob The shader blob pointer.
			 */
			void createDXILLibrary(CD3DX12_STATE_OBJECT_DESC& stateObject, ID3DBlob* pShaderBlob) const;

		private:
			ComPtr<ID3D12RootSignature> m_RootSignature;
			ComPtr<ID3D12StateObject> m_PipelineState;
		};
	}
}