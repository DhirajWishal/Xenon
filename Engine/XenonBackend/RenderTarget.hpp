// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Camera.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Attachment type enum.
		 *
		 * Note that both depth and depth with stencil cannot be set at the same time!
		 */
		enum class AttachmentType : uint8_t
		{
			Color = XENON_BIT_SHIFT(0),					// Used for color output. Shader output: layout(location = 0) out vec4
			EntityID = XENON_BIT_SHIFT(1),				// Commonly used for mouse picking. Shader output: layout(location = 1) out float
			Normal = XENON_BIT_SHIFT(2),				// Used for normal output. Shader output: layout(location = 2) out vec3

			Depth = XENON_BIT_SHIFT(8),					// Used for depth information.
			DepthWithStencil = XENON_BIT_SHIFT(7)		// Used for depth information. It also contains a stencil component.
		};

		XENON_DEFINE_ENUM_AND(AttachmentType);
		XENON_DEFINE_ENUM_OR(AttachmentType);

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
			explicit RenderTarget(Device* pDevice, Camera* pCamera, AttachmentType attachmentTypes) : m_pCamera(pCamera), m_AttachmentTypes(attachmentTypes) {}

			/**
			 * Default virtual destructor.
			 */
			virtual ~RenderTarget() = default;

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

		private:
			Camera* m_pCamera = nullptr;

			AttachmentType m_AttachmentTypes;
		};
	}
}