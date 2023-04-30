// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/RayTracingPipeline.hpp"

#include "DX12CommandRecorder.hpp"
#include "DX12DescriptorHeapManager.hpp"
#include "DX12Buffer.hpp"

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
			 * @param specification The pipeline specification.
			 */
			explicit DX12RayTracingPipeline(DX12Device* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, const RayTracingPipelineSpecification& specification);

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

			/**
			 * Create a new shader binding table.
			 *
			 * @param bindingGroups The binding groups.
			 * @return The created shader binding table.
			 */
			[[nodiscard]] std::unique_ptr<ShaderBindingTable> createShaderBindingTable(const std::vector<BindingGroup>& bindingGroups) override;

			/**
			 * Get the pipeline state object pointer.
			 *
			 * @return The state object pointer.
			 */
			[[nodiscard]] ID3D12StateObject* getStateObject() noexcept { return m_PipelineState.Get(); }

			/**
			 * Get the pipeline state object pointer.
			 *
			 * @return The state object pointer.
			 */
			[[nodiscard]] const ID3D12StateObject* getStateObject() const noexcept { return m_PipelineState.Get(); }

			/**
			 * Get the shader ID of a shader.
			 *
			 * @param type The shader type.
			 * @param group The group of the shader.
			 */
			[[nodiscard]] void* getShaderID(ShaderType type, uint32_t group) const;

		private:
			/**
			 * Create the DXIL library.
			 *
			 * @param stateObject The state object to create the sub-object from.
			 * @param shader The shader bytecode.
			 * @param newExport The new export name. This is needed as all the shader export will be "main".
			 */
			void createDXILLibrary(CD3DX12_STATE_OBJECT_DESC& stateObject, D3D12_SHADER_BYTECODE shader, const std::wstring_view& newExport) const;

			/**
			 * Create a local root signature.
			 *
			 * @param rangePairs The descriptor range maps.
			 * @return The created root signature raw pointer.
			 */
			[[nodiscard]] ID3D12RootSignature* createLocalRootSignature(std::vector<std::pair<uint8_t, std::vector<CD3DX12_DESCRIPTOR_RANGE1>>>&& rangePairs);

			/**
			 * Create the global root signature.
			 *
			 * @param rangePairs The descriptor range maps.
			 */
			void createGlobalRootSignature(std::vector<std::pair<uint8_t, std::vector<CD3DX12_DESCRIPTOR_RANGE1>>>&& rangePairs);

		private:
			std::unordered_map<DescriptorType, std::unordered_map<uint32_t, UINT>> m_BindingOffsets;

			std::vector<ComPtr<ID3D12RootSignature>> m_LocalRootSignatures;
			ComPtr<ID3D12RootSignature> m_GlobalRootSignature;
			ComPtr<ID3D12StateObject> m_PipelineState;
			ComPtr<ID3D12StateObjectProperties> m_PipelineStateProperties;

			uint64_t m_PipelineHash = 0;
		};
	}
}