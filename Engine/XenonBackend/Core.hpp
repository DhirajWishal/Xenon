// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonCore/Common.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Multi sampling count enum.
		 */
		enum class MultiSamplingCount : uint8_t
		{
			x1 = XENON_BIT_SHIFT(0),
			x2 = XENON_BIT_SHIFT(1),
			x4 = XENON_BIT_SHIFT(2),
			x8 = XENON_BIT_SHIFT(3),
			x16 = XENON_BIT_SHIFT(4),
			x32 = XENON_BIT_SHIFT(5),
			x64 = XENON_BIT_SHIFT(6)
		};

		/**
		 * Attachment type enum.
		 */
		enum class AttachmentType : uint8_t
		{
			Color = XENON_BIT_SHIFT(0),					// Used for color output. Shader output: layout(location = 0) out vec4
			EntityID = XENON_BIT_SHIFT(1),				// Commonly used for mouse picking. Shader output: layout(location = 1) out float
			Normal = XENON_BIT_SHIFT(2),				// Used for normal output. Shader output: layout(location = 2) out vec3

			Depth = XENON_BIT_SHIFT(6),					// Used for depth information.
			Stencil = XENON_BIT_SHIFT(7),				// Used for stencil information.
		};

		XENON_DEFINE_ENUM_AND(AttachmentType);
		XENON_DEFINE_ENUM_OR(AttachmentType);

		/**
		 * Data format enum.
		 */
		enum class DataFormat : uint8_t
		{
			Undefined,

			R8_SRGB,
			R8G8_SRGB,
			R8G8B8_SRGB,
			R8G8B8A8_SRGB,

			R8_UNORMAL,
			R8G8_UNORMAL,
			R8G8B8_UNORMAL,
			R8G8B8A8_UNORMAL,

			B8G8R8_SRGB,
			B8G8R8A8_SRGB,

			B8G8R8_UNORMAL,
			B8G8R8A8_UNORMAL,

			R16_SFLOAT,
			R16G16_SFLOAT,
			R16G16B16_SFLOAT,
			R16G16B16A16_SFLOAT,

			R32_SFLOAT,
			R32G32_SFLOAT,
			R32G32B32_SFLOAT,
			R32G32B32A32_SFLOAT,

			D16_SINT,
			D32_SFLOAT,

			S8_UINT,
			D16_UNORMAL_S8_UINT,
			D24_UNORMAL_S8_UINT,
			D32_SFLOAT_S8_UINT,
		};

		/**
		 * Image type enum.
		 */
		enum class ImageType : uint8_t
		{
			OneDimensional,
			TwoDimensional,
			ThreeDImentional,
			CubeMap
		};

		/**
		 * Image usage enum.
		 * This specifies the possible image usages.
		 */
		enum class ImageUsage : uint8_t
		{
			Storage = XENON_BIT_SHIFT(0),
			Graphics = XENON_BIT_SHIFT(1),
			ColorAttachment = XENON_BIT_SHIFT(2),
			DepthAttachment = XENON_BIT_SHIFT(3)
		};

		XENON_DEFINE_ENUM_AND(ImageUsage);
		XENON_DEFINE_ENUM_OR(ImageUsage);
	}
}