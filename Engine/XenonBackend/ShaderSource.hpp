// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core.hpp"

#include <filesystem>

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
			 * Default constructor.
			 */
			ShaderSource() = default;

			/**
			 * Explicit constructor.
			 *
			 * @param binary The binary data to set.
			 * @param entryPoint The shader's entry point. Default is main.
			 */
			explicit ShaderSource(const BinaryType& binary, const std::string& entryPoint = "main");

			/**
			 * Explicit constructor.
			 *
			 * @param binary The binary data to set.
			 * @param entryPoint The shader's entry point. Default is main.
			 */
			explicit ShaderSource(BinaryType&& binary, const std::string& entryPoint = "main");

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
			 * Get the binary data pointer.
			 *
			 * @return The data pointer.
			 */
			[[nodiscard]] const uint32_t* getBinaryData() const noexcept { return m_Binary.data(); }

			/**
			 * Get the shader binary size.
			 *
			 * @return The size of the binary container.
			 */
			[[nodiscard]] uint64_t getBinarySize() const noexcept { return m_Binary.size(); }

			/**
			 * Get the shader binary size in bytes.
			 *
			 * @return The size of the binary container in bytes.
			 */
			[[nodiscard]] uint64_t getBinarySizeInBytes() const noexcept { return m_Binary.size() * sizeof(uint32_t); }

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

		private:
			BinaryType m_Binary;
			std::string m_EntryPoint;
		};
	}

	/**
	 * Utility function to easily generate the hash for the shader object.
	 *
	 * @param source The shader source to generate the hash for.
	 * @param seed The hash seed. Default is 0.
	 * @return The 64-bit hash value.
	 */
	template<>
	[[nodiscard]] inline uint64_t GenerateHashFor<Backend::ShaderSource>(const Backend::ShaderSource& source, uint64_t seed) noexcept
	{
		return GenerateHash(ToBytes(source.getBinaryData()), source.getBinarySizeInBytes(), seed);
	}
}