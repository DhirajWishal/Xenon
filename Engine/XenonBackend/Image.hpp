// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Data format enum.
		 */
		enum class DataFormat : uint8_t
		{
			Undefined
		};

		/**
		 * Image type enum.
		 */
		enum class ImageType : uint8_t
		{
			Storage,

			Texture2D,
			Texture3D,
			CubeMap,

			ColorAttachment,
			EntityIDAttachment,
			NormalAttachment,
			DepthAttachment,
			DepthStencilAttachment
		};

		/**
		 * Xenon image class.
		 * This class is used to store information about an image, which can be of multiple layers.
		 */
		class Image : public BackendObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param width The width of the image.
			 * @param height The height of the image.
			 * @param depth The depth of the image.
			 * @param layers The number of layers in the image.
			 * @param type The image type. This is defined by the backend images.
			 * @param format The image's pixel format.
			 * @param enableMipMapping Whether to enable mip-mapping or not. Default is true.
			 */
			explicit Image(Device* pDevice, uint32_t width, uint32_t height, uint32_t depth, uint32_t layers, DataFormat format, ImageType type, bool enableMipMapping = true)
				: m_Width(width), m_Height(height), m_Depth(depth), m_Format(format) {}

			/**
			 * Default virtual destructor.
			 */
			virtual ~Image() = default;

		private:
			uint32_t m_Width = 0;
			uint32_t m_Height = 0;
			uint32_t m_Depth = 0;
			uint32_t m_Layers = 0;

			DataFormat m_Format = DataFormat::Undefined;
		};
	}
}