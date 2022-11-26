// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonCore/Common.hpp"

#include <vector>

#include <glm/vec3.hpp>

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
			for (auto i = (sizeof(std::underlying_type_t<DataFormat>) * 8) - 1; i > 0; i--)
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
		 * Input element enum.
		 *
		 * Input elements are of two types.
		 * 1. Vertex elements. These are the ones that are stored in vertex buffers.
		 * 2. Instance elements. These are the ones that are stored in instance buffers.
		 */
		enum class InputElement : uint8_t
		{
			// Stored and used as a 3 component vector.
			// Access this from GLSL: layout(location = 0) in vec3
			VertexPosition,

			// Stored and used as a 3 component vector. 
			// Access this from GLSL: layout(location = 1) in vec3
			VertexNormal,

			// Stored and used as a 3 component vector. 
			// Access this from GLSL: layout(location = 2) in vec3
			VertexTangent,

			// Stored and used as a 4 component vector. 
			// Access this from GLSL: layout(location = 3) in vec4
			VertexColor_0,

			// Stored and used as a 4 component vector. 
			// Access this from GLSL: layout(location = 4) in vec4
			VertexColor_1,

			// Stored and used as a 4 component vector. 
			// Access this from GLSL: layout(location = 5) in vec4
			VertexColor_2,

			// Stored and used as a 4 component vector. 
			// Access this from GLSL: layout(location = 6) in vec4
			VertexColor_3,

			// Stored and used as a 4 component vector. 
			// Access this from GLSL: layout(location = 7) in vec4
			VertexColor_4,

			// Stored and used as a 4 component vector. 
			// Access this from GLSL: layout(location = 8) in vec4
			VertexColor_5,

			// Stored and used as a 4 component vector. 
			// Access this from GLSL: layout(location = 9) in vec4
			VertexColor_6,

			// Stored and used as a 4 component vector. 
			// Access this from GLSL: layout(location = 10) in vec4
			VertexColor_7,

			// Stored and used as a 2 component vector. 
			// Access this from GLSL: layout(location = 11) in vec2
			VertexTextureCoordinate_0,

			// Stored and used as a 2 component vector. 
			// Access this from GLSL: layout(location = 12) in vec2
			VertexTextureCoordinate_1,

			// Stored and used as a 2 component vector. 
			// Access this from GLSL: layout(location = 13) in vec2
			VertexTextureCoordinate_2,

			// Stored and used as a 2 component vector. 
			// Access this from GLSL: layout(location = 14) in vec2
			VertexTextureCoordinate_3,

			// Stored and used as a 2 component vector. 
			// Access this from GLSL: layout(location = 15) in vec2
			VertexTextureCoordinate_4,

			// Stored and used as a 2 component vector. 
			// Access this from GLSL: layout(location = 16) in vec2
			VertexTextureCoordinate_5,

			// Stored and used as a 2 component vector. 
			// Access this from GLSL: layout(location = 17) in vec2
			VertexTextureCoordinate_6,

			// Stored and used as a 2 component vector. 
			// Access this from GLSL: layout(location = 18) in vec2
			VertexTextureCoordinate_7,

			// Stored and used as a 4 component vector. 
			// Access this from GLSL: layout(location = 19) in vec4
			VertexJointIndices,

			// Stored and used as a 4 component vector. 
			// Access this from GLSL: layout(location = 20) in vec4
			VertexJointWeight,

			// Stored and used as a 3 component vector. 
			// Access this from GLSL: layout(location = 21) in vec3
			InstancePosition,

			// Stored and used as a 3 component vector. 
			// Access this from GLSL: layout(location = 22) in vec3
			InstanceRotation,

			// Stored and used as a 3 component vector. 
			// Access this from GLSL: layout(location = 23) in vec3
			InstanceScale,

			// Stored and used as an integer.
			// Access this from GLSL: layout(location = 24) in int
			InstanceID,

			// This is just a count of the number of supported elements.
			Count,

			// This is just a count of the number of supported vertex elements.
			VertexElementCount = VertexJointWeight - VertexPosition + 1,

			// This is just a count of the number of supported instance elements.
			InstanceElementCount = InstanceID - InstancePosition + 1,

			// Undefined element.
			Undefined = -1
		};

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
		 * Get the number of components an attribute data type has.
		 *
		 * @param type The type of the attribute.
		 * @return The component count.
		 */
		[[nodiscard]] constexpr uint8_t GetAttributeDataTypeComponentCount(AttributeDataType type) noexcept
		{
			switch (type)
			{
			case Xenon::Backend::AttributeDataType::Vec2:
				return 2;

			case Xenon::Backend::AttributeDataType::Vec3:
				return 3;

			case Xenon::Backend::AttributeDataType::Vec4:
				return 4;

			case Xenon::Backend::AttributeDataType::Mat2:
				return 4;

			case Xenon::Backend::AttributeDataType::Mat3:
				return 9;

			case Xenon::Backend::AttributeDataType::Mat4:
				return 16;

			case Xenon::Backend::AttributeDataType::Scalar:
				return 1;

			default:
				return 0;
			}
		}

		/**
		 * Check if an input element is a vertex element.
		 *
		 * @param element The element to check.
		 * @return True if the element is a vertex element.
		 * @return False if the element is not a vertex element.
		 */
		[[nodiscard]] constexpr bool IsVertexElement(InputElement element) noexcept
		{
			return EnumToInt(element) <= EnumToInt(InputElement::VertexJointWeight);
		}

		/**
		 * Check if an input element is an instance element.
		 *
		 * @param element The element to check.
		 * @return True if the element is an instance element.
		 * @return False if the element is not an instance element.
		 */
		[[nodiscard]] constexpr bool IsInstanceElement(InputElement element) noexcept
		{
			return EnumToInt(element) >= EnumToInt(InputElement::InstancePosition) && EnumToInt(element) <= EnumToInt(InputElement::InstanceID);
		}

		/**
		 * Get the input element data types.
		 *
		 * @param element The element to get the data type of.
		 * @return The data type.
		 */
		[[nodiscard]] constexpr AttributeDataType GetInputElementDataType(InputElement element)
		{
			switch (element)
			{
			case Xenon::Backend::InputElement::VertexPosition:
			case Xenon::Backend::InputElement::VertexNormal:
			case Xenon::Backend::InputElement::VertexTangent:
				return AttributeDataType::Vec3;

			case Xenon::Backend::InputElement::VertexColor_0:
			case Xenon::Backend::InputElement::VertexColor_1:
			case Xenon::Backend::InputElement::VertexColor_2:
			case Xenon::Backend::InputElement::VertexColor_3:
			case Xenon::Backend::InputElement::VertexColor_4:
			case Xenon::Backend::InputElement::VertexColor_5:
			case Xenon::Backend::InputElement::VertexColor_6:
			case Xenon::Backend::InputElement::VertexColor_7:
				return AttributeDataType::Vec4;

			case Xenon::Backend::InputElement::VertexTextureCoordinate_0:
			case Xenon::Backend::InputElement::VertexTextureCoordinate_1:
			case Xenon::Backend::InputElement::VertexTextureCoordinate_2:
			case Xenon::Backend::InputElement::VertexTextureCoordinate_3:
			case Xenon::Backend::InputElement::VertexTextureCoordinate_4:
			case Xenon::Backend::InputElement::VertexTextureCoordinate_5:
			case Xenon::Backend::InputElement::VertexTextureCoordinate_6:
			case Xenon::Backend::InputElement::VertexTextureCoordinate_7:
				return AttributeDataType::Vec2;

			case Xenon::Backend::InputElement::VertexJointIndices:
			case Xenon::Backend::InputElement::VertexJointWeight:
				return AttributeDataType::Vec4;

			case Xenon::Backend::InputElement::InstancePosition:
			case Xenon::Backend::InputElement::InstanceRotation:
			case Xenon::Backend::InputElement::InstanceScale:
				return AttributeDataType::Vec3;

			case Xenon::Backend::InputElement::InstanceID:
				return AttributeDataType::Scalar;

			default:
				return AttributeDataType::Scalar;
			}
		}

		/**
		 * Vertex specification class.
		 * This contains information about a single vertex including it's size, and the actual elements that are been stored.
		 */
		class VertexSpecification final
		{
		public:
			/**
			 * Default constructor.
			 */
			constexpr VertexSpecification() = default;

			/**
			 * Add a vertex element to the specification.
			 *
			 * @param element The element to add.
			 * @param componentSize The size of a single component in the element in bytes. Default is sizeof(float).
			 * @return The specification reference.
			 */
			VertexSpecification& addElement(InputElement element, uint8_t componentSize = sizeof(float))
			{
				const auto size = componentSize * GetAttributeDataTypeComponentCount(GetInputElementDataType(element));

				// Update the information only if it's needed.
				if (!isAvailable(element) || m_ElementSizes[EnumToInt(element)] != size)
				{
					m_VertexElements |= 1 << EnumToInt(element);
					m_ElementSizes[EnumToInt(element)] = size;
				}

				return *this;
			}

			/**
			 * Get the size of a single element.
			 *
			 * @param element The element to get the size of.
			 * @return The element's size.
			 */
			[[nodiscard]] uint8_t getElementSize(InputElement element) const { return m_ElementSizes[EnumToInt(element)]; }

			/**
			 * Get the size of the vertex.
			 *
			 * @return The size in bytes.
			 */
			[[nodiscard]] uint32_t getSize() const noexcept
			{
				uint32_t size = 0;
				for (const auto elementSize : m_ElementSizes)
					size += elementSize;

				return size;
			}

			/**
			 * Check if a vertex element is present in the vertex.
			 *
			 * @param element The element to check.
			 * @return True if the element is present.
			 * @return False if the element is not present.
			 */
			[[nodiscard]] bool isAvailable(InputElement element) const noexcept { return m_VertexElements & (1 << EnumToInt(element)); }

		private:
			uint32_t m_VertexElements = 0;
			std::array<uint8_t, EnumToInt(InputElement::VertexElementCount)> m_ElementSizes = { 0 };
		};

		/**
		 * Instance entry structure.
		 * This contains information regarding a single instance entry in the instance buffer.
		 */
		struct InstanceEntry final
		{
			glm::vec3 m_Position = {};	// InputElement::InstancePosition
			glm::vec3 m_Rotation = {};	// InputElement::InstanceRotation
			glm::vec3 m_Scale = {};		// InputElement::InstanceScale

			uint32_t m_InstanceID = 0;	// InputElement::InstanceID
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