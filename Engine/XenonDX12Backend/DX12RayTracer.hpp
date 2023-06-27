// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/RayTracer.hpp"

#include "DX12Image.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * DirectX 12 ray tracer class.
		 */
		class DX12RayTracer final : public RayTracer, public DX12DeviceBoundObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param width The width of the render target.
			 * @param height The height of the render target.
			 */
			explicit DX12RayTracer(DX12Device* pDevice, uint32_t width, uint32_t height);

			/**
			 * Destructor.
			 */
			~DX12RayTracer() override = default;

			/**
			 * Get the image attachment of the relevant attachment type.
			 *
			 * @param type The attachment type.
			 * @return The attachment image.
			 */
			XENON_NODISCARD Image* getImageAttachment(AttachmentType type) override;

		private:
			DX12Image m_ColorImage;
		};
	}
}