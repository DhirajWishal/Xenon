// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/RayTracingPipeline.hpp"

#include "DX12DescriptorHeapManager.hpp"
#include "DX12Buffer.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Shader binding table class.
		 */
		class ShaderBindingTable final : public DX12DeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 */
			explicit ShaderBindingTable(DX12Device* pDevice) : DX12DeviceBoundObject(pDevice) {}

			/**
			 * Add a shader ID to the table.
			 *
			 * @param pID The shader ID.
			 */
			void addShaderID(void* pID) { m_ShaderIDs.emplace_back(pID); }

			/**
			 * Create the shader binding table buffer.
			 *
			 * @param rootParameterCount The number of root parameters.
			 */
			void create(UINT rootParameterCount);

		private:
			std::vector<void*> m_ShaderIDs;
			std::unique_ptr<DX12Buffer> m_pShaderBindingTable = nullptr;
		};

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
			 * Get the ray gen shader binding table.
			 *
			 * @return The shader binding table.
			 */
			[[nodiscard]] ShaderBindingTable& getRayGenSBT() noexcept { return m_RayGenSBT; }

			/**
			 * Get the ray gen shader binding table.
			 *
			 * @return The shader binding table.
			 */
			[[nodiscard]] const ShaderBindingTable& getRayGenSBT() const noexcept { return m_RayGenSBT; }

			/**
			 * Get the miss shader binding table.
			 *
			 * @return The shader binding table.
			 */
			[[nodiscard]] ShaderBindingTable& ge_MissSBT() noexcept { return m_MissSBT; }

			/**
			 * Get the miss shader binding table.
			 *
			 * @return The shader binding table.
			 */
			[[nodiscard]] const ShaderBindingTable& ge_MissSBT() const noexcept { return m_MissSBT; }

			/**
			 * Get the hit group shader binding table.
			 *
			 * @return The shader binding table.
			 */
			[[nodiscard]] ShaderBindingTable& getHitGroupSBT() noexcept { return m_HitGroupSBT; }

			/**
			 * Get the hit group shader binding table.
			 *
			 * @return The shader binding table.
			 */
			[[nodiscard]] const ShaderBindingTable& getHitGroupSBT() const noexcept { return m_HitGroupSBT; }

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

			/**
			 * Create the shader binding table.
			 */
			void createShaderBindingTable();

			/**
			 * Get the root argument count of a shader.
			 *
			 * @param shader The shader to get the root argument count of.
			 * @return The root argument count.
			 */
			[[nodiscard]] uint32_t getRootArgumentCount(const Shader& shader) const noexcept;

		private:
			ShaderBindingTable m_RayGenSBT;
			ShaderBindingTable m_MissSBT;
			ShaderBindingTable m_HitGroupSBT;

			std::unique_ptr<DX12Buffer> m_pShaderBindingTable = nullptr;

			std::vector<ComPtr<ID3D12RootSignature>> m_LocalRootSignatures;
			ComPtr<ID3D12RootSignature> m_GlobalRootSignature;
			ComPtr<ID3D12StateObject> m_PipelineState;

			uint64_t m_PipelineHash = 0;
		};
	}
}