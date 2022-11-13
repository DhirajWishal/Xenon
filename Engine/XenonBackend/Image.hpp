// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Image specification structure.
		 * This contains all the necessary information to create an image.
		 */
		struct ImageSpecification final
		{
			uint32_t m_Width = 0;
			uint32_t m_Height = 0;
			uint32_t m_Depth = 1;
			uint32_t m_Layers = 1;

			ImageType m_Type = ImageType::TwoDimensional;
			ImageUsage m_Usage = ImageUsage::Graphics;
			DataFormat m_Format = DataFormat::Undefined;
			MultiSamplingCount m_MultiSamplingCount = MultiSamplingCount::x1;

			bool m_EnableMipMaps = true;
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
			 * @param specification The image specification.
			 */
			explicit Image([[maybe_unused]] Device* pDevice, const ImageSpecification& specification) : m_Specification(specification) {}

			/**
			 * Move constructor.
			 *
			 * @param other The other image.
			 */
			Image(Image&& other) noexcept : m_Specification(other.m_Specification) {}

			/**
			 * Default virtual destructor.
			 */
			virtual ~Image() = default;

			/**
			 * Get the image specification.
			 *
			 * @return The image specification.
			 */
			[[nodiscard]] const ImageSpecification& getSpecification() const { return m_Specification; }

		public:
			/**
			 * Move assignment operator.
			 *
			 * @param other The other image.
			 * @return The move-assigned image.
			 */
			Image& operator=(Image&& other) noexcept { m_Specification = other.m_Specification; return *this; }

		protected:
			ImageSpecification m_Specification = {};
		};
	}
}