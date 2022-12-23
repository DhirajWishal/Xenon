// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/OcclusionQuery.hpp"

#include "DX12DeviceBoundObject.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * DirectX 12 occlusion query class.
		 */
		class DX12OcclusionQuery final : public OcclusionQuery, public DX12DeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param sampleCount The number of possible samples.
			 */
			explicit DX12OcclusionQuery(DX12Device* pDevice, uint64_t sampleCount);

			/**
			 * Destructor.
			 */
			~DX12OcclusionQuery() override;

			/**
			 * Get the results form the query.
			 *
			 * @return The samples
			 */
			[[nodiscard]] const std::vector<uint64_t>& getResults() override;

			/**
			 * Get the occlusion query heap.
			 *
			 * @return The heap pointer.
			 */
			[[nodiscard]] ID3D12QueryHeap* getHeap() noexcept { return m_QueryHeap.Get(); }

			/**
			 * Get the occlusion query heap.
			 *
			 * @return The heap pointer.
			 */
			[[nodiscard]] const ID3D12QueryHeap* getHeap() const noexcept { return m_QueryHeap.Get(); }

		private:
			ComPtr<ID3D12QueryHeap> m_QueryHeap;
			ComPtr<ID3D12Resource> m_QueryBuffer;

			D3D12MA::Allocation* m_pAllocation = nullptr;
		};
	}
}