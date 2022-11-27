// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/RasterizingPipeline.hpp"

#include "DX12Rasterizer.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Direct X 12 rasterizing pipeline class.
		 */
		class DX12RasterizingPipeline final : public RasterizingPipeline, public DX12DeviceBoundObject
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
			~DX12RasterizingPipeline() override = default;

			/**
			 * Get a pipeline using the vertex specification.
			 * If a pipeline does not exist for the given vertex specification, it'll create a new one.
			 *
			 * @param vertexSpecification The vertex specification.
			 * @return The pipeline storage.
			 */
			[[nodiscard]] const PipelineStorage& getPipeline(const VertexSpecification& vertexSpecification);

		private:
			/**
			 * Create the root signature.
			 *
			 * @param rangeMap The descriptor range map.
			 */
			void createRootSignature(std::unordered_map<uint8_t, std::vector<CD3DX12_DESCRIPTOR_RANGE1>>&& rangeMap);

			/**
			 * Setup the pipeline state descriptor for future use.
			 */
			void setupPipelineStateDescriptor();

		private:
			D3D12_GRAPHICS_PIPELINE_STATE_DESC m_PipelineStateDescriptor = {};

			std::unordered_map<uint64_t, PipelineStorage> m_Pipelines;
			std::vector<D3D12_INPUT_ELEMENT_DESC> m_Inputs;

			ComPtr<ID3D12RootSignature> m_RootSignature;
			ComPtr<ID3DBlob> m_VertexShader;
			ComPtr<ID3DBlob> m_PixelShader;

			DX12Rasterizer* m_pRasterizer = nullptr;
		};
	}
}