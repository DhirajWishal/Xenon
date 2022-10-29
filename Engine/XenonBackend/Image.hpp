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
			 * Default virtual destructor.
			 */
			virtual ~Image() = default;

		private:
			ImageSpecification m_Specification = {};
		};
	}
}