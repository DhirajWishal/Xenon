// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonCore/Common.hpp"

#include <vector>

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
		 *
		 * Formats can be binary-OR-ed to add multiple candidate formats. In that case the best available format is used by the backend.
		 */
		enum class DataFormat : uint32_t
		{
			Undefined = 0,

			R8_SRGB = XENON_BIT_SHIFT(0),
			R8_UNORMAL = XENON_BIT_SHIFT(1),
			R16_SFLOAT = XENON_BIT_SHIFT(2),
			R32_SFLOAT = XENON_BIT_SHIFT(3),

			R8G8_SRGB = XENON_BIT_SHIFT(4),
			R8G8_UNORMAL = XENON_BIT_SHIFT(5),
			R16G16_SFLOAT = XENON_BIT_SHIFT(6),
			R32G32_SFLOAT = XENON_BIT_SHIFT(7),

			R8G8B8_SRGB = XENON_BIT_SHIFT(8),
			R8G8B8_UNORMAL = XENON_BIT_SHIFT(9),
			R16G16B16_SFLOAT = XENON_BIT_SHIFT(10),
			R32G32B32_SFLOAT = XENON_BIT_SHIFT(11),

			B8G8R8_SRGB = XENON_BIT_SHIFT(12),
			B8G8R8_UNORMAL = XENON_BIT_SHIFT(13),

			R8G8B8A8_SRGB = XENON_BIT_SHIFT(14),
			R8G8B8A8_UNORMAL = XENON_BIT_SHIFT(15),
			R16G16B16A16_SFLOAT = XENON_BIT_SHIFT(16),
			R32G32B32A32_SFLOAT = XENON_BIT_SHIFT(17),

			B8G8R8A8_SRGB = XENON_BIT_SHIFT(18),
			B8G8R8A8_UNORMAL = XENON_BIT_SHIFT(19),

			D16_SINT = XENON_BIT_SHIFT(20),
			D32_SFLOAT = XENON_BIT_SHIFT(21),

			S8_UINT = XENON_BIT_SHIFT(22),

			D16_UNORMAL_S8_UINT = XENON_BIT_SHIFT(23),
			D24_UNORMAL_S8_UINT = XENON_BIT_SHIFT(24),
			D32_SFLOAT_S8_UINT = XENON_BIT_SHIFT(25),
		};

		XENON_DEFINE_ENUM_AND(DataFormat);
		XENON_DEFINE_ENUM_OR(DataFormat);

		/**
		 * Get the number of candidate formats in the format.
		 *
		 * @param format The format to check.
		 * @return The number of candidate formats. The count will be 0 if undefined.
		 */
		[[nodiscard]] constexpr uint32_t CountCandiateFormats(DataFormat format)
		{
			uint32_t count = 0;
			if (format == DataFormat::Undefined)
				return count;

			using UnderlyingType = std::underlying_type_t<DataFormat>;
			for (UnderlyingType i = 0; i < sizeof(UnderlyingType) * 8; i++)
			{
				if (EnumToInt(format) & (1 << i))
					count++;
			}

			return count;
		}

		/**
		 * Get the candidate formats from the format list.
		 *
		 * @param format The format with candidates.
		 * @return The candidate format vector with the most to least important order.
		 */
		[[nodiscard]] constexpr std::vector<DataFormat> GetCandidateFormats(DataFormat format)
		{
			std::vector<DataFormat> candidates;
			for (auto i = sizeof(std::underlying_type_t<DataFormat>) * 8; i > 0; i--)
			{
				if (EnumToInt(format) & (1 << i))
					candidates.push_back(static_cast<DataFormat>(1 << i));
			}

			return candidates;
		}

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
		 *
		 * Note that the image usage cannot be color and depth at the same time! If both are specified it is considered as a color image.
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

		/**
		 * Attribute data type.
		 * This specifies which data type an attribute uses.
		 */
		enum class AttributeDataType : uint8_t
		{
			Vec2,
			Vec3,
			Vec4,

			Mat2,
			Mat3,
			Mat4,

			Scalar
		};

		/**
		 * Resource type enum.
		 */
		enum class ResourceType : uint8_t
		{
			Sampler,
			CombinedImageSampler,

			SampledImage,
			StorageImage,

			UniformTexelBuffer,
			StorageTexelBuffer,

			UniformBuffer,
			StorageBuffer,

			DynamicUniformBuffer,
			DynamicStorageBuffer,

			InputAttachment,

			AccelerationStructure
		};

		/**
		 * Descriptor type enum.
		 */
		enum class DescriptorType : uint8_t
		{
			UserDefined,	// This descriptor type contains any other user defined data, like the model matrix.
			Material,		// This descriptor type contains all the material-specific information.
			Camera			// This descriptor type only has one binding (0) which passes the camera data.
		};

		/**
		 * Shader type enum.
		 */
		enum class ShaderType : uint16_t
		{
			Vertex = XENON_BIT_SHIFT(0),
			Fragment = XENON_BIT_SHIFT(1),	// Also known as the pixel shader in DirectX

			RayGen = XENON_BIT_SHIFT(2),
			Intersection = XENON_BIT_SHIFT(3),
			AnyHit = XENON_BIT_SHIFT(4),
			ClosestHit = XENON_BIT_SHIFT(5),
			Miss = XENON_BIT_SHIFT(6),
			Callable = XENON_BIT_SHIFT(7),

			Compute = XENON_BIT_SHIFT(8)
		};

		XENON_DEFINE_ENUM_AND(ShaderType);
		XENON_DEFINE_ENUM_OR(ShaderType);

		/**
		 * Descriptor binding information structure.
		 */
		struct DescriptorBindingInfo final
		{
			ShaderType m_ApplicableShaders = ShaderType::Vertex;	// Bitwise-OR all the applicable shaders.
			ResourceType m_Type = ResourceType::Sampler;
		};
	}
}