// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/RasterizingPipeline.hpp"

#include "DX12DescriptorHeapManager.hpp"
#include "DX12Rasterizer.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Direct X 12 rasterizing pipeline class.
		 */
		class DX12RasterizingPipeline final : public RasterizingPipeline, public DX12DescriptorHeapManager
		{
			/**
			 * Pipeline storage structure.
			 */
			struct PipelineStorage final
			{
				std::vector<D3D12_INPUT_ELEMENT_DESC> m_Inputs;
				ComPtr<ID3D12PipelineState> m_PipelineState;
			};

		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param pCacheHandler The cache handler pointer.
			 * @param pRasterizer The rasterizer pointer.
			 * @param specification The pipeline specification.
			 */
			explicit DX12RasterizingPipeline(DX12Device* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler, DX12Rasterizer* pRasterizer, const RasterizingPipelineSpecification& specification);

			/**
			 * Destructor.
			 */
			~DX12RasterizingPipeline() override;

			/**
			 * Create a new descriptor.
			 *
			 * @param type The descriptor type to create.
			 * @return The descriptor pointer. It will return nullptr if the descriptor type is not present in the pipeline.
			 */
			XENON_NODISCARD std::unique_ptr<Descriptor> createDescriptor(DescriptorType type) override;

			/**
			 * Get a pipeline using the vertex specification.
			 * If a pipeline does not exist for the given vertex specification, it'll create a new one.
			 *
			 * @param vertexSpecification The vertex specification.
			 * @return The pipeline storage.
			 */
			XENON_NODISCARD const PipelineStorage& getPipeline(const VertexSpecification& vertexSpecification);

			/**
			 * Get the root signature.
			 *
			 * @return The root signature.
			 */
			XENON_NODISCARD ID3D12RootSignature* getRootSignature() { return m_RootSignature.Get(); }

			/**
			 * Get the root signature.
			 *
			 * @return The root signature.
			 */
			XENON_NODISCARD const ID3D12RootSignature* getRootSignature() const { return m_RootSignature.Get(); }

		private:
			/**
			 * Create the root signature.
			 *
			 * @param rangePairs The descriptor range pairs.
			 */
			void createRootSignature(std::vector<std::pair<uint8_t, std::vector<CD3DX12_DESCRIPTOR_RANGE1>>>&& rangePairs);

			/**
			 * Setup the pipeline state descriptor for future use.
			 */
			void setupPipelineStateDescriptor();

			/**
			 * Load the pipeline cache.
			 *
			 * @param hash The pipeline hash.
			 * @return The pipeline cache.
			 */
			XENON_NODISCARD std::vector<std::byte> loadPipelineStateCache(uint64_t hash) const;

			/**
			 * Store the pipeline cache.
			 *
			 * @param hash The pipeline hash.
			 * @param pipeline The pipeline to store the cache data from.
			 */
			void storePipelineStateCache(uint64_t hash, const PipelineStorage& pipeline) const;

		private:
			D3D12_GRAPHICS_PIPELINE_STATE_DESC m_PipelineStateDescriptor = {};

			std::mutex m_Mutex;

			std::unordered_map<DescriptorType, std::unordered_map<uint32_t, UINT>> m_BindingOffsets;
			std::unordered_map<uint64_t, PipelineStorage> m_Pipelines;
			std::vector<D3D12_INPUT_ELEMENT_DESC> m_Inputs;

			ComPtr<ID3D12RootSignature> m_RootSignature;
			ComPtr<ID3DBlob> m_VertexShader;
			ComPtr<ID3DBlob> m_PixelShader;

			DX12Rasterizer* m_pRasterizer = nullptr;
		};
	}
}