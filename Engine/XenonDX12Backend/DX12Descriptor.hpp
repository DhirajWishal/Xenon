// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/Descriptor.hpp"

#include "DX12DescriptorHeapManager.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * DirectX 12 descriptor class.
		 */
		class DX12Descriptor final : public Descriptor, public DX12DeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param bindingInfo The descriptor's binding information. Make sure that the binding information are in the binding order (the first one is binging 0, second is 1 and so on).
			 * @param type The descriptor type.
			 * @param bindingOffsets The binding offsets describing the heap offsets.
			 * @param pManager The descriptor manager pointer.
			 */
			explicit DX12Descriptor(DX12Device* pDevice, const std::unordered_map<uint32_t, DescriptorBindingInfo>& bindingInfo, DescriptorType type, const std::unordered_map<uint32_t, UINT>& bindingOffsets, DX12DescriptorHeapManager* pManager);

			/**
			 * Destructor.
			 */
			~DX12Descriptor() override;

			/**
			 * Attach a buffer to the descriptor.
			 *
			 * @param binding The binding of the buffer.
			 * @param pBuffer The buffer to attach.
			 */
			void attach(uint32_t binding, Buffer* pBuffer) override;

			/**
			 * Attach an image to the descriptor.
			 *
			 * @param binding The binding of the image.
			 * @param pImage The image to attach.
			 * @param pView The image view.
			 * @param pSampler The image sampler.
			 * @param usage How the image is used in the binding.
			 */
			void attach(uint32_t binding, Image* pImage, ImageView* pView, ImageSampler* pSampler, ImageUsage usage) override;

			/**
			 * Get the CBV, SRV and UAV descriptor heap start.
			 *
			 * @return The heap start.
			 */
			XENON_NODISCARD UINT getCbvSrvUavDescriptorHeapStart() const { return m_CbvSrvUavDescriptorHeapStart; }

			/**
			 * Get the CBV, SRV and UAV descriptor heap increment size.
			 *
			 * @return The heap increment size.
			 */
			XENON_NODISCARD UINT getCbvSrvUavDescriptorHeapIncrementSize() const { return m_pManager->getCbvSrvUavHeapIncrementSize(); }

			/**
			 * Get the sampler descriptor heap start.
			 *
			 * @return The heap start.
			 */
			XENON_NODISCARD UINT getSamplerDescriptorHeapStart() const { return m_SamplerDescriptorHeapStart; }

			/**
			 * Get the sampler descriptor heap increment size.
			 *
			 * @return The heap increment size.
			 */
			XENON_NODISCARD UINT getSamplerDescriptorHeapIncrementSize() const { return m_pManager->getSamplerHeapIncrementSize(); }

			/**
			 * Check if the descriptor has buffers.
			 *
			 * @return True if the descriptor has buffers.
			 * @return False if the descriptor does not have buffers.
			 */
			XENON_NODISCARD bool hasBuffers() const { return m_pManager->getGroupSize(m_Type).first > 0; }

			/**
			 * Check if the descriptor has samplers.
			 *
			 * @return True if the descriptor has samplers.
			 * @return False if the descriptor does not have samplers.
			 */
			XENON_NODISCARD bool hasSampler() const { return m_pManager->getGroupSize(m_Type).second > 0; }

		public:
			std::unordered_map<uint32_t, UINT> m_BindingOffsets;

			DX12DescriptorHeapManager* m_pManager = nullptr;

			UINT m_CbvSrvUavDescriptorHeapStart = 0;
			UINT m_SamplerDescriptorHeapStart = 0;
		};
	}
}