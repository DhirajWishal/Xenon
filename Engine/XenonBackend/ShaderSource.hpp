// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core.hpp"

#include <filesystem>

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Shader attribute structure.
		 */
		struct ShaderAttribute final
		{
			uint32_t m_Location = 0;
			AttributeDataType m_DataType = AttributeDataType::Scalar;
		};

		/**
		 * Shader resource structure.
		 */
		struct ShaderResource final
		{
			uint32_t m_Binding = 0;

			DescriptorType m_Set = DescriptorType::Camera;
			ResourceType m_Type = ResourceType::Sampler;
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
			 * Default constructor.
			 */
			ShaderSource() = default;

			/**
			 * Explicit constructor.
			 *
			 * @param binary The binary data to set.
			 * @param entryPoint The shader's entry point. Default is main.
			 * @param shouldPerformReflection Whether to perform reflection or not. Default is true. Set this to false if we're storing anything other than SPIR-V!
			 */
			explicit ShaderSource(const BinaryType& binary, const std::string& entryPoint = "main", bool shouldPerformReflection = true);

			/**
			 * Explicit constructor.
			 *
			 * @param binary The binary data to set.
			 * @param entryPoint The shader's entry point. Default is main.
			 * @param shouldPerformReflection Whether to perform reflection or not. Default is true. Set this to false if we're storing anything other than SPIR-V!
			 */
			explicit ShaderSource(BinaryType&& binary, const std::string& entryPoint = "main", bool shouldPerformReflection = true);

			/**
			 * Load the shader source from a source file.
			 *
			 * @param shader The shader source file.
			 * @param entryPoint The shader's entry point. Default is main.
			 * @return The shader source object.
			 */
			[[nodiscard]] static ShaderSource FromFile(const std::filesystem::path& shader, const std::string& entryPoint = "main");

			/**
			 * Get the shader binary.
			 *
			 * @return The shader binary.
			 */
			[[nodiscard]] const BinaryType& getBinary() const noexcept { return m_Binary; }

			/**
			 * Get the shader binary without the last padding bytes.
			 *
			 * @return The shader binary without padding.
			 */
			[[nodiscard]] BinaryType getBinaryWithoutPadding() const { return std::vector<uint32_t>(m_Binary.begin(), m_Binary.begin() + (m_Binary.size() / 4)); }

			/**
			 * Get the shader's entry point.
			 *
			 * @return The entry point.
			 */
			[[nodiscard]] std::string_view getEntryPoint() const { return m_EntryPoint; }

			/**
			 * Check if the shader source is valid or not.
			 * A shader source is valid if we have some binary data.
			 *
			 * @return True if the shader source is valid.
			 * @return False if the shader source is not valid.
			 */
			[[nodiscard]] bool isValid() const noexcept { return !m_Binary.empty(); }

			/**
			 * Get the shader's input attributes.
			 *
			 * @return The input attributes.
			 */
			[[nodiscard]] const std::vector<ShaderAttribute>& getInputAttributes() const noexcept { return m_InputAttributes; }

			/**
			 * Get the shader's output attributes.
			 *
			 * @return The output attributes.
			 */
			[[nodiscard]] const std::vector<ShaderAttribute>& getOutputAttributes() const noexcept { return m_OutputAttributes; }

			/**
			 * Get the resources.
			 *
			 * @return The shader resources.
			 */
			[[nodiscard]] const std::vector<ShaderResource>& getResources() const noexcept { return m_Resources; }

		private:
			/**
			 * Perform reflection over the binary source and get information about inputs, outputs and resources.
			 */
			void performReflection();

		private:
			BinaryType m_Binary;
			std::string m_EntryPoint;

			std::vector<ShaderAttribute> m_InputAttributes;
			std::vector<ShaderAttribute> m_OutputAttributes;
			std::vector<ShaderResource> m_Resources;
		};
	}
}