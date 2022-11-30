// Copyright 2022 Dhiraj Wishal
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
			 * @param pCamera The camera which is used to render the scene.
			 * @param attachmentTypes The attachment types the render target should support.
			 */
			explicit RenderTarget([[maybe_unused]] const Device* pDevice, Camera* pCamera, AttachmentType attachmentTypes) : m_pCamera(pCamera), m_AttachmentTypes(attachmentTypes) {}

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
			[[nodiscard]] AttachmentType getAttachmentTypes() const { return m_AttachmentTypes; }

			/**
			 * Get the camera pointer.
			 *
			 * @return The camera pointer.
			 */
			[[nodiscard]] Camera* getCamera() { return m_pCamera; }

			/**
			 * Get the camera pointer.
			 *
			 * @return The const camera pointer.
			 */
			[[nodiscard]] const Camera* getCamera() const { return m_pCamera; }

		protected:
			Camera* m_pCamera = nullptr;

			AttachmentType m_AttachmentTypes;
		};
	}
}