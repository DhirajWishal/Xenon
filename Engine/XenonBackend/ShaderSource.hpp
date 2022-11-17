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
		};
	}
}