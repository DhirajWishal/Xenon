// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Camera.hpp"
#include "Image.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Xenon render target class.
		 * Render targets are used to render information to an image. This output image(s) can be used for other purposes.
		 */
		class RenderTarget : public BackendObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param width The width of the render target.
			 * @param height The height of the render target.
			 * @param attachmentTypes The attachment types the render target should support.
			 */
			explicit RenderTarget([[maybe_unused]] const Device* pDevice, uint32_t width, uint32_t height, AttachmentType attachmentTypes) : m_Width(width), m_Height(height), m_AttachmentTypes(attachmentTypes) {}

			/**
			 * Get the image attachment of the relevant attachment type.
			 *
			 * @param type The attachment type.
			 * @return The attachment image.
			 */
			[[nodiscard]] virtual Image* getImageAttachment(AttachmentType type) = 0;

		public:
			/**
			 * Get the attachment types supported by the render target.
			 *
			 * @return The attachment types.
			 */
			[[nodiscard]] AttachmentType getAttachmentTypes() const noexcept { return m_AttachmentTypes; }

			/**
			 * Get the width of the render target.
			 *
			 * @return The width.
			 */
			[[nodiscard]] uint32_t getWidth() const noexcept { return m_Width; }

			/**
			 * Get the height of the render target.
			 *
			 * @return The height.
			 */
			[[nodiscard]] uint32_t getHeight() const noexcept { return m_Height; }

		protected:
			uint32_t m_Width = 0;
			uint32_t m_Height = 0;

			AttachmentType m_AttachmentTypes;
		};
	}
}