// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/Image.hpp"

#include "DX12DeviceBoundObject.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Direct X 12 image class.
		 */
		class DX12Image final : public Image, public DX12DeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param specification The image specification.
			 * @param resourceStates The image's resource states. Default is copy destination.
			 * @param heapType The memory heap type. Default is default.
			 * @param heapFlags The heap flags. Default is none.
			 */
			explicit DX12Image(
				DX12Device* pDevice,
				const ImageSpecification& specification,
				D3D12_RESOURCE_STATES resourceStates = D3D12_RESOURCE_STATE_COPY_DEST,
				D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_DEFAULT,
				D3D12_HEAP_FLAGS heapFlags = D3D12_HEAP_FLAG_NONE);

			/**
			 * Move constructor.
			 *
			 * @param other The other image.
			 */
			DX12Image(DX12Image&& other) noexcept;

			/**
			 * Default destructor.
			 */
			~DX12Image() override;

			/**
			 * Copy image data from a source buffer.
			 *
			 * @param pSrcBuffer The source buffer pointer.
			 */
			void copyFrom(Buffer* pSrcBuffer) override;

			/**
			 * Get the backend resource.
			 *
			 * @return The resource pointer.
			 */
			[[nodiscard]] ID3D12Resource* getResource() { return m_pAllocation->GetResource(); }

			/**
			 * Get the backend resource.
			 *
			 * @return The const resource pointer.
			 */
			[[nodiscard]] const ID3D12Resource* getResource() const { return m_pAllocation->GetResource(); }

			/**
			 * Get the image's quality levels.
			 * 
			 * @return The quality levels.
			 */
			[[nodiscard]] UINT getQualityLevel() const { return m_QualityLevel; }

		public:
			/**
			 * Move assign operator.
			 *
			 * @param other The other image.
			 * @return The assigned image reference.
			 */
			DX12Image& operator=(DX12Image&& other) noexcept;

		private:
			D3D12MA::Allocation* m_pAllocation = nullptr;

			ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
			ComPtr<ID3D12GraphicsCommandList> m_CommandList;

			D3D12_RESOURCE_STATES m_CurrentState;
			UINT m_QualityLevel = 0;
		};
	}
}