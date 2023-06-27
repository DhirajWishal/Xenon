// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonCore/Common.hpp"

#include <vector>
#include <array>

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
			Color = XENON_BIT_SHIFT(0),					// Used for color output. Shader output: This is a 4 component image.
			EntityID = XENON_BIT_SHIFT(1),				// Commonly used for mouse picking. This is a 1 component image.
			Normal = XENON_BIT_SHIFT(2),				// Used for normal output. This is a 4 component image.
			Position = XENON_BIT_SHIFT(3),				// Used for position output. This is a 4 component image, the last being the depth.

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
		XENON_NODISCARD constexpr uint32_t CountCandiateFormats(DataFormat format)
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

#ifdef XENON_FEATURE_CONSTEXPR_VECTOR
		/**
		 * Get the candidate formats from the format list.
		 *
		 * @param format The format with candidates.
		 * @return The candidate format vector with the most to least important order.
		 */
		XENON_NODISCARD constexpr std::vector<DataFormat> GetCandidateFormats(DataFormat format)
		{
			std::vector<DataFormat> candidates;
			for (auto i = (sizeof(std::underlying_type_t<DataFormat>) * 8) - 1; i > 0; i--)
			{
				if (EnumToInt(format) & (1 << i))
					candidates.push_back(static_cast<DataFormat>(1 << i));
			}

			return candidates;
		}

#else
		/**
		 * Get the candidate formats from the format list.
		 *
		 * @param format The format with candidates.
		 * @return The candidate format vector with the most to least important order.
		 */
		XENON_NODISCARD std::vector<DataFormat> GetCandidateFormats(DataFormat format)
		{
			std::vector<DataFormat> candidates;
			for (auto i = (sizeof(std::underlying_type_t<DataFormat>) * 8) - 1; i > 0; i--)
			{
				if (EnumToInt(format) & (1 << i))
					candidates.push_back(static_cast<DataFormat>(1 << i));
			}

			return candidates;
		}

#endif

		/**
		 * Check if the format is a depth format.
		 *
		 * @param format The data format.
		 * @return True if the format is a depth format.
		 * @return False if the format is not a depth format.
		 */
		XENON_NODISCARD constexpr bool IsDepthFormat(DataFormat format) noexcept
		{
			switch (format)
			{
			case Xenon::Backend::DataFormat::Undefined:
			case Xenon::Backend::DataFormat::R8_SRGB:
			case Xenon::Backend::DataFormat::R8_UNORMAL:
			case Xenon::Backend::DataFormat::R16_SFLOAT:
			case Xenon::Backend::DataFormat::R32_SFLOAT:
			case Xenon::Backend::DataFormat::R8G8_SRGB:
			case Xenon::Backend::DataFormat::R8G8_UNORMAL:
			case Xenon::Backend::DataFormat::R16G16_SFLOAT:
			case Xenon::Backend::DataFormat::R32G32_SFLOAT:
			case Xenon::Backend::DataFormat::R8G8B8_SRGB:
			case Xenon::Backend::DataFormat::R8G8B8_UNORMAL:
			case Xenon::Backend::DataFormat::R16G16B16_SFLOAT:
			case Xenon::Backend::DataFormat::R32G32B32_SFLOAT:
			case Xenon::Backend::DataFormat::B8G8R8_SRGB:
			case Xenon::Backend::DataFormat::B8G8R8_UNORMAL:
			case Xenon::Backend::DataFormat::R8G8B8A8_SRGB:
			case Xenon::Backend::DataFormat::R8G8B8A8_UNORMAL:
			case Xenon::Backend::DataFormat::R16G16B16A16_SFLOAT:
			case Xenon::Backend::DataFormat::R32G32B32A32_SFLOAT:
			case Xenon::Backend::DataFormat::B8G8R8A8_SRGB:
			case Xenon::Backend::DataFormat::B8G8R8A8_UNORMAL:
			case Xenon::Backend::DataFormat::S8_UINT:
				return false;

			case Xenon::Backend::DataFormat::D16_SINT:
			case Xenon::Backend::DataFormat::D32_SFLOAT:
			case Xenon::Backend::DataFormat::D16_UNORMAL_S8_UINT:
			case Xenon::Backend::DataFormat::D24_UNORMAL_S8_UINT:
			case Xenon::Backend::DataFormat::D32_SFLOAT_S8_UINT:
				return true;

			default:
				break;
			}

			return false;
		}

		/**
		 * Check if the data format has a stencil component.
		 *
		 * @param format The data format to check.
		 * @return True if the format has a stencil component.
		 * @return False if the format doesn't have a stencil component.
		 */
		XENON_NODISCARD constexpr bool HasStencilComponent(DataFormat format) noexcept
		{
			switch (format)
			{
			case Xenon::Backend::DataFormat::Undefined:
			case Xenon::Backend::DataFormat::R8_SRGB:
			case Xenon::Backend::DataFormat::R8_UNORMAL:
			case Xenon::Backend::DataFormat::R16_SFLOAT:
			case Xenon::Backend::DataFormat::R32_SFLOAT:
			case Xenon::Backend::DataFormat::R8G8_SRGB:
			case Xenon::Backend::DataFormat::R8G8_UNORMAL:
			case Xenon::Backend::DataFormat::R16G16_SFLOAT:
			case Xenon::Backend::DataFormat::R32G32_SFLOAT:
			case Xenon::Backend::DataFormat::R8G8B8_SRGB:
			case Xenon::Backend::DataFormat::R8G8B8_UNORMAL:
			case Xenon::Backend::DataFormat::R16G16B16_SFLOAT:
			case Xenon::Backend::DataFormat::R32G32B32_SFLOAT:
			case Xenon::Backend::DataFormat::B8G8R8_SRGB:
			case Xenon::Backend::DataFormat::B8G8R8_UNORMAL:
			case Xenon::Backend::DataFormat::R8G8B8A8_SRGB:
			case Xenon::Backend::DataFormat::R8G8B8A8_UNORMAL:
			case Xenon::Backend::DataFormat::R16G16B16A16_SFLOAT:
			case Xenon::Backend::DataFormat::R32G32B32A32_SFLOAT:
			case Xenon::Backend::DataFormat::B8G8R8A8_SRGB:
			case Xenon::Backend::DataFormat::B8G8R8A8_UNORMAL:
			case Xenon::Backend::DataFormat::D16_SINT:
			case Xenon::Backend::DataFormat::D32_SFLOAT:
				return false;

			case Xenon::Backend::DataFormat::S8_UINT:
			case Xenon::Backend::DataFormat::D16_UNORMAL_S8_UINT:
			case Xenon::Backend::DataFormat::D24_UNORMAL_S8_UINT:
			case Xenon::Backend::DataFormat::D32_SFLOAT_S8_UINT:
				return true;

			default:
				break;
			}

			return false;
		}

		/**
		 * Image type enum.
		 */
		enum class ImageType : uint8_t
		{
			OneDimensional,
			TwoDimensional,
			ThreeDimensional,
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
		 * Image state enum.
		 */
		enum class ImageState : uint8_t
		{
			Undefined,				// Undefined image state type. This is the first state right after creating an image.
			General,				// General image state type. This might not be efficient but is required in some places. Let the backend handle it...

			TransferSource,			// Prepare the image to copy data from to another image (to transfer destination).
			TransferDestination,	// Prepare the image to be copied to from another image (from transfer source).

			ShaderRead,				// Prepare the image to be read by a shader. 
			ShaderWrite,			// Prepare the image to be written by a shader.

			AttachmentRead,			// Prepare the image to be written by a render target. This could be depth or color depending on the image usage.
			AttachmentWrite			// Prepare the image to be read as an attachment. This could be depth or color depending on the image usage.
		};

		/**
		 * Input element enum.
		 *
		 * Input elements are of two types.
		 * 1. Vertex elements. These are the ones that are stored in vertex buffers.
		 * 2. Instance elements. These are the ones that are stored in instance buffers.
		 */
		enum class InputElement : uint8_t
		{
			// Access this from GLSL: layout(location = 0) in
			VertexPosition,

			// Access this from GLSL: layout(location = 1) in
			VertexNormal,

			// Access this from GLSL: layout(location = 2) in
			VertexTangent,

			// Access this from GLSL: layout(location = 3) in
			VertexColor_0,

			// Access this from GLSL: layout(location = 4) in
			VertexColor_1,

			// Access this from GLSL: layout(location = 5) in
			VertexColor_2,

			// Access this from GLSL: layout(location = 6) in
			VertexColor_3,

			// Access this from GLSL: layout(location = 7) in
			VertexColor_4,

			// Access this from GLSL: layout(location = 8) in
			VertexColor_5,

			// Access this from GLSL: layout(location = 9) in
			VertexColor_6,

			// Access this from GLSL: layout(location = 10) in
			VertexColor_7,

			// Access this from GLSL: layout(location = 11) in
			VertexTextureCoordinate_0,

			// Access this from GLSL: layout(location = 12) in
			VertexTextureCoordinate_1,

			// Access this from GLSL: layout(location = 13) in
			VertexTextureCoordinate_2,

			// Access this from GLSL: layout(location = 14) in
			VertexTextureCoordinate_3,

			// Access this from GLSL: layout(location = 15) in
			VertexTextureCoordinate_4,

			// Access this from GLSL: layout(location = 16) in
			VertexTextureCoordinate_5,

			// Access this from GLSL: layout(location = 17) in
			VertexTextureCoordinate_6,

			// Access this from GLSL: layout(location = 18) in
			VertexTextureCoordinate_7,

			// Access this from GLSL: layout(location = 19) in
			VertexJointIndices,

			// Access this from GLSL: layout(location = 20) in
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
			Undefined = UINT8_MAX
		};

		/**
		 * Check if an input element is a vertex element.
		 *
		 * @param element The element to check.
		 * @return True if the element is a vertex element.
		 * @return False if the element is not a vertex element.
		 */
		XENON_NODISCARD constexpr bool IsVertexElement(InputElement element) noexcept
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
		XENON_NODISCARD constexpr bool IsInstanceElement(InputElement element) noexcept
		{
			return EnumToInt(element) >= EnumToInt(InputElement::InstancePosition) && EnumToInt(element) <= EnumToInt(InputElement::InstanceID);
		}

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
		XENON_NODISCARD constexpr uint8_t GetAttributeDataTypeComponentCount(AttributeDataType type) noexcept
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
		 * Component data type enum.
		 * This defines information about a single component data type.
		 */
		enum class ComponentDataType : uint8_t
		{
			Void,

			Uint8,
			Uint16,
			Uint32,
			Uint64,

			Int8,
			Int16,
			Int32,
			Int64,

			Float,
			Double
		};

		/**
		 * Get the component type size.
		 *
		 * @type The component type.
		 * @return The byte size of the component.
		 */
		XENON_NODISCARD constexpr uint8_t GetComponentTypeSize(ComponentDataType type) noexcept
		{
			switch (type)
			{
			case Xenon::Backend::ComponentDataType::Uint8:
				return sizeof(uint8_t);

			case Xenon::Backend::ComponentDataType::Uint16:
				return sizeof(uint16_t);

			case Xenon::Backend::ComponentDataType::Uint32:
				return sizeof(uint32_t);

			case Xenon::Backend::ComponentDataType::Uint64:
				return sizeof(uint64_t);

			case Xenon::Backend::ComponentDataType::Int8:
				return sizeof(int8_t);

			case Xenon::Backend::ComponentDataType::Int16:
				return sizeof(int16_t);

			case Xenon::Backend::ComponentDataType::Int32:
				return sizeof(int32_t);

			case Xenon::Backend::ComponentDataType::Int64:
				return sizeof(int64_t);

			case Xenon::Backend::ComponentDataType::Float:
				return sizeof(float);

			case Xenon::Backend::ComponentDataType::Double:
				return sizeof(double);

			default:
				return 0;
			}
		}

		/**
		 * Vertex specification class.
		 * This contains information about a single vertex including it's size, and the actual elements that are been stored.
		 */
		class VertexSpecification final
		{
			/**
			 * Vertex element structure.
			 */
			struct VertexElement final
			{
				InputElement m_Element = InputElement::Undefined;

				uint8_t m_Size = 0;
				uint8_t m_Offset = 0;

				AttributeDataType m_AttributeDataType = AttributeDataType::Vec2;
				ComponentDataType m_ComponentDataType = ComponentDataType::Void;
			};

		public:
#ifdef XENON_FEATURE_CONSTEXPR_VECTOR
			/**
			 * Default constructor.
			 */
			constexpr VertexSpecification() = default;

#else
			/**
			 * Default constructor.
			 */
			VertexSpecification() = default;

#endif

			/**
			 * Add a vertex element to the specification.
			 * Note that offsets are calculated internally so this function must be called in the same order they appear in a vertex.
			 * Adding the same element twice might lead to errors!
			 *
			 * @param element The element to add.
			 * @param dataType The element's data type.
			 * @param componentSize The size of a single component in the element in bytes. Default is Float.
			 * @return The specification reference.
			 */
			VertexSpecification& addElement(InputElement element, AttributeDataType dataType, ComponentDataType componentSize = ComponentDataType::Float)
			{

				// Update the information only if it's needed.
				if (!isAvailable(element))
				{
					m_ElementIndexMap[EnumToInt(element)] = static_cast<uint8_t>(m_Elements.size());

					uint8_t offset = 0;
					if (!m_Elements.empty())
						offset = m_Elements.back().m_Offset + m_Elements.back().m_Size;

					m_Elements.emplace_back(element, GetComponentTypeSize(componentSize) * GetAttributeDataTypeComponentCount(dataType), offset, dataType, componentSize);

					m_VertexElements |= 1 << EnumToInt(element);
				}

				return *this;
			}

			/**
			 * Get the offset of an element.
			 *
			 * @param element The element to get the offset of.
			 * @return The offset in bytes.
			 */
			XENON_NODISCARD uint8_t offsetOf(InputElement element) const { return m_Elements[m_ElementIndexMap[EnumToInt(element)]].m_Offset; }

			/**
			 * Get the size of a single element.
			 *
			 * @param element The element to get the size of.
			 * @return The element's size.
			 */
			XENON_NODISCARD uint8_t getElementSize(InputElement element) const { return m_Elements[m_ElementIndexMap[EnumToInt(element)]].m_Size; }

			/**
			 * Get the element component data type of a given element.
			 *
			 * @param element The element type.
			 * @return The component data type.
			 */
			XENON_NODISCARD ComponentDataType getElementComponentDataType(InputElement element) const { return m_Elements[m_ElementIndexMap[EnumToInt(element)]].m_ComponentDataType; }

			/**
			 * Get the element attribute data type of a given element.
			 *
			 * @param element The element type.
			 * @return The attribute data type.
			 */
			XENON_NODISCARD AttributeDataType getElementAttributeDataType(InputElement element) const { return m_Elements[m_ElementIndexMap[EnumToInt(element)]].m_AttributeDataType; }

			/**
			 * Get the size of the vertex.
			 *
			 * @return The size in bytes.
			 */
			XENON_NODISCARD uint32_t getSize() const noexcept
			{
				uint32_t size = 0;
				for (const auto& element : m_Elements)
					size += element.m_Size;

				return size;
			}

			/**
			 * Check if a vertex element is present in the vertex.
			 *
			 * @param element The element to check.
			 * @return True if the element is present.
			 * @return False if the element is not present.
			 */
			XENON_NODISCARD bool isAvailable(InputElement element) const noexcept { return m_VertexElements & (1 << EnumToInt(element)); }

			/**
			 * Generate hash for the vertex specification.
			 *
			 * @return The hash value.
			 */
			XENON_NODISCARD uint64_t generateHash() const { return GenerateHash(ToBytes(m_Elements.data()), sizeof(VertexElement) * m_Elements.size(), m_VertexElements); }

		private:
			uint32_t m_VertexElements = 0;
			std::array<uint8_t, EnumToInt(InputElement::VertexElementCount)> m_ElementIndexMap = { 0 };
			std::vector<VertexElement> m_Elements;
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
		 * Index buffer stride enum.
		 */
		enum class IndexBufferStride : uint8_t
		{
			Uint16 = sizeof(uint16_t),
			Uint32 = sizeof(uint32_t)
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
		 * User defined bindings enum.
		 * This contains all the binding types with their corresponding binding in the user defined descriptor.
		 * This is used since some of the binding slots are used by internal buffers (and/ or images).
		 */
		enum class PerGeometryBindings : uint32_t
		{
			Transform
		};

		/**
		 * Scene bindings enum.
		 * This contains all the binding types with their corresponding bindings in the scene descriptor.
		 */
		enum class SceneBindings : uint32_t
		{
			SceneInformation,
			Camera,

			LightSources,

			// Used for ray tracing.
			AccelerationStructure,

			// Used for ray tracing.
			RenderTarget
		};

		/**
		 * Resource operation enum.
		 * This describes what types of operations the shader does to the resource.
		 */
		enum class ResouceOperation : uint8_t
		{
			Read = XENON_BIT_SHIFT(0),
			Write = XENON_BIT_SHIFT(1)
		};

		XENON_DEFINE_ENUM_AND(ResouceOperation);
		XENON_DEFINE_ENUM_OR(ResouceOperation);

		/**
		 * Descriptor type enum.
		 * These descriptors are organized by (potentially) the most changing to less changing per draw call.
		 */
		enum class DescriptorType : uint8_t
		{
			UserDefined,	// This descriptor type contains any other user defined (custom) data.
			Material,		// This descriptor type contains all the material-specific information.
			PerGeometry,	// This descriptor type contains all the per-geometry information, like transform and others.
			Scene			// This descriptor type contains all the scene related information, like the camera, lighting, acceleration structures, etc...
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