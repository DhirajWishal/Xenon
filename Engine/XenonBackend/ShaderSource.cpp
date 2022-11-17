// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ShaderSource.hpp"
#include "../XenonCore/Logging.hpp"

#include <fstream>

namespace Xenon
{
	namespace Backend
	{
		Xenon::Backend::ShaderSource ShaderSource::FromFile(const std::filesystem::path& shader)
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
				shaderFile.read(reinterpret_cast<char*>(binaryData.data()), size);

				// Close the file now.
				shaderFile.close();
			}
			else
			{
				XENON_LOG_ERROR("Failed to load the shader source @{}", shader.string());
			}

			return ShaderSource(std::move(binaryData));
		}
	}
}