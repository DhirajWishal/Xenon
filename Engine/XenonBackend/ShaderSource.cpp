// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#include "ShaderSource.hpp"
#include "../XenonCore/Logging.hpp"

#include <fstream>

namespace Xenon
{
	namespace Backend
	{
		ShaderSource::ShaderSource(const BinaryType& binary, const std::string& entryPoint /*= "main"*/)
			: m_Binary(binary)
			, m_EntryPoint(entryPoint)
		{
		}

		ShaderSource::ShaderSource(BinaryType&& binary, const std::string& entryPoint /*= "main"*/)
			: m_Binary(std::move(binary))
			, m_EntryPoint(entryPoint)
		{
		}

		Xenon::Backend::ShaderSource ShaderSource::FromFile(const std::filesystem::path& shader, const std::string& entryPoint /*= "main"*/)
		{
			std::vector<uint32_t> binaryData;
			auto shaderFile = std::fstream(shader, std::ios::in | std::ios::binary | std::ios::ate);

			// Load the data if we were able to open the file.
			if (shaderFile.is_open())
			{
				// Get the file size.
				const auto size = shaderFile.tellg();
				shaderFile.seekg(0);

				// Load the data to the vector.
				binaryData.resize(size);
				shaderFile.read(std::bit_cast<char*>(binaryData.data()), size);

				// Close the file now.
				shaderFile.close();
			}
			else
			{
				XENON_LOG_ERROR("Failed to load the shader source @{}", shader.string());
			}

			return ShaderSource(std::move(binaryData), entryPoint);
		}
	}
}