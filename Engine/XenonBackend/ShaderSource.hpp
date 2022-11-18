// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <filesystem>
#include <vector>

namespace Xenon
{
	namespace Backend
	{
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
		 * Shader resource structure.
		 */
		struct ShaderResource final
		{
			uint32_t m_Binding = 0;
			uint32_t m_Set = 0;

			ResourceType m_Type = ResourceType::Sampler;
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
		 * Shader attribute structure.
		 */
		struct ShaderAttribute final
		{
			uint32_t m_Location = 0;
			AttributeDataType m_DataType = AttributeDataType::Scalar;
		};

		/**
		 * Shader source class.
		 * Shaders are programs that are run on the GPU for specific tasks. This class contains the shader code in the
		 * SPIR-V format (it will be converted to the appropriate format from the backend).
		 */
		class ShaderSource final
		{
			using BinaryType = std::vector<uint32_t>;

		public:
			/**
			 * Explicit constructor.
			 *
			 * @param binary The binary data to set.
			 */
			explicit ShaderSource(const BinaryType& binary) : m_Binary(binary) {}

			/**
			 * Explicit constructor.
			 *
			 * @param binary The binary data to set.
			 */
			explicit ShaderSource(BinaryType&& binary) : m_Binary(std::move(binary)) {}

			/**
			 * Perform reflection over the binary source and get information about inputs, outputs and resources.
			 */
			void performReflection();

			/**
			 * Load the shader source from a source file.
			 *
			 * @param shader The shader source file.
			 * @return The shader source object.
			 */
			[[nodiscard]] static ShaderSource FromFile(const std::filesystem::path& shader);

			/**
			 * Get the shader binary.
			 *
			 * @return The shader binary.
			 */
			[[nodiscard]] const BinaryType& getBinary() const { return m_Binary; }

		private:
			BinaryType m_Binary;

			std::vector<ShaderResource> m_Resources;
			std::vector<ShaderAttribute> m_InputAttributes;
			std::vector<ShaderAttribute> m_OutputAttributes;
		};
	}
}