// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Address mode enum.
		 */
		enum class AddressMode : uint8_t
		{
			Repeat,
			MirroredRepeat,
			ClampToEdge,
			ClampToBorder,
			MirrorClampToEdge,
		};

		/**
		 * Border color enum.
		 */
		enum class BorderColor : uint8_t
		{
			TransparentBlackFLOAT,
			TransparentBlackINT,
			OpaqueBlackFLOAT,
			OpaqueBlackINT,
			OpaqueWhiteFLOAT,
			OpaqueWhiteINT,
		};

		/**
		 * Compare operator enum.
		 */
		enum class CompareOperator : uint8_t
		{
			Never,
			Less,
			Equal,
			LessOrEqual,
			Greater,
			NotEqual,
			GreaterOrEqual,
			Always,
		};

		/**
		 * Image filter enum.
		 */
		enum class ImageFilter : uint8_t
		{
			Nearest,
			Linear,
			CubicImage,
		};

		/**
		 *Image mip map mode enum.
		 */
		enum class ImageMipMapMode : uint8_t
		{
			Nearest,
			Linear,
		};

		/**
		 * Image sampler specification structure.
		 */
		struct ImageSamplerSpecification final
		{
			// If set to 0.0f, the maximum supported will be set.
			float m_MaxAnisotrophy = 0.0f;

			float m_MaxLevelOfDetail = 0.0f;
			float m_MinLevelOfDetail = 0.0f;

			float m_MipLevelOfDetailBias = 0.0f;

			AddressMode m_AddressModeU = AddressMode::Repeat;
			AddressMode m_AddressModeV = AddressMode::Repeat;
			AddressMode m_AddressModeW = AddressMode::Repeat;

			BorderColor m_BorderColor = BorderColor::OpaqueWhiteFLOAT;

			CompareOperator m_CompareOperator = CompareOperator::Always;

			ImageFilter m_ImageMagificationFilter = ImageFilter::Linear;
			ImageFilter m_ImageMinificationFilter = ImageFilter::Linear;

			ImageMipMapMode m_MipMapMode = ImageMipMapMode::Linear;

			bool m_bEnableAnisotropy = true;
			bool m_bEnableCompare = false;
			bool m_bEnableUnnormalizedCoordinates = false;
		};

		/**
		 * Image sampler class.
		 * This class specifies how to sample an image from the shader.
		 */
		class ImageSampler : public BackendObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param specification The sampler specification.
			 */
			explicit ImageSampler([[maybe_unused]] Device* pDevice, const ImageSamplerSpecification& specification) : m_Specification(specification) {}

			/**
			 * Get the image sampler specification.
			 *
			 * @return The image sampler specification.
			 */
			[[nodiscard]] const ImageSamplerSpecification& getSpecification() const { return m_Specification; }

		private:
			ImageSamplerSpecification m_Specification = {};
		};
	}
}