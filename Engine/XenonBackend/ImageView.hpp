// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Image.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Component swizzle enum.
		 */
		enum class ComponentSwizzle : uint8_t
		{
			Identity,
			Zero,
			One,

			R,
			G,
			B,
			A
		};

		/**
		 * Image view specification structure.
		 */
		struct ImageViewSpecification final
		{
			uint32_t m_BaseMipLevel = 0;
			uint32_t m_LevelCount = 1;
			uint32_t m_BaseArrayLayer = 0;
			uint32_t m_LayerCount = 1;

			ComponentSwizzle m_ComponentR = ComponentSwizzle::Identity;
			ComponentSwizzle m_ComponentG = ComponentSwizzle::Identity;
			ComponentSwizzle m_ComponentB = ComponentSwizzle::Identity;
			ComponentSwizzle m_ComponentA = ComponentSwizzle::Identity;
		};

		/**
		 * Image view class.
		 * This class contains a single image's resource view.
		 */
		class ImageView : public BackendObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param pImage The image pointer.
			 * @param specification The view specification.
			 */
			explicit ImageView([[maybe_unused]] const Device* pDevice, [[maybe_unused]] const Image* pImage, const ImageViewSpecification& specification) : m_Specification(specification) {}

			/**
			 * Get the image view specification.
			 *
			 * @return The image view specification.
			 */
			XENON_NODISCARD const ImageViewSpecification& getSpecification() const { return m_Specification; }

		private:
			ImageViewSpecification m_Specification = {};
		};
	}
}