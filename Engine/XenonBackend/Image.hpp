// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.hpp"

namespace Xenon
{
	namespace Backend
	{
		class Buffer;

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
			explicit Image(XENON_MAYBE_UNUSED const Device* pDevice, const ImageSpecification& specification) : m_Specification(specification) {}

			/**
			 * Move constructor.
			 *
			 * @param other The other image.
			 */
			Image(Image&& other) noexcept : m_Specification(other.m_Specification) {}

			/**
			 * Copy image data from a source buffer.
			 *
			 * @param pSrcBuffer The source buffer pointer.
			 * @param pCommandRecorder The command recorder pointer to record the commands to. Default is nullptr.
			 */
			virtual void copyFrom(Buffer* pSrcBuffer, CommandRecorder* pCommandRecorder = nullptr) = 0;

			/**
			 * Copy image data from a source image.
			 *
			 * @param pSrcImage The source image.
			 * @param pCommandRecorder The command recorder pointer to record the commands to. Default is nullptr.
			 */
			virtual void copyFrom(Image* pSrcImage, CommandRecorder* pCommandRecorder = nullptr) = 0;

			/**
			 * Generate mip maps for the currently stored image.
			 *
			 * @param pCommandRecorder The command recorder pointer to record the commands to. Default is nullptr (in which case the backend will create one for this purpose).
			 */
			virtual void generateMipMaps(CommandRecorder* pCommandRecorder = nullptr) = 0;

			/**
			 * Get the image specification.
			 *
			 * @return The image specification.
			 */
			XENON_NODISCARD const ImageSpecification& getSpecification() const noexcept { return m_Specification; }

			/**
			 * Get the width of the image.
			 *
			 * @return The image's width.
			 */
			XENON_NODISCARD uint32_t getWidth() const noexcept { return m_Specification.m_Width; }

			/**
			 * Get the height of the image.
			 *
			 * @return The image's height.
			 */
			XENON_NODISCARD uint32_t getHeight() const noexcept { return m_Specification.m_Height; }

			/**
			 * Get the depth of the image.
			 *
			 * @return The image's depth.
			 */
			XENON_NODISCARD uint32_t getDepth() const noexcept { return m_Specification.m_Depth; }

			/**
			 * Get the image's data format.
			 *
			 * @return The format.
			 */
			XENON_NODISCARD DataFormat getDataFormat() const noexcept { return m_Specification.m_Format; }

			/**
			 * Get the image usage.
			 *
			 * @return The image usage.
			 */
			XENON_NODISCARD ImageUsage getUsage() const noexcept { return m_Specification.m_Usage; }

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