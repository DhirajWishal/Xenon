// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "MeshStorage.hpp"

#include <rapidjson/document.h>
#include <fstream>

namespace /* anonymous */
{
	/**
	 * Load the RapidJSON document from the file.
	 *
	 * @param file The file to load the JSON data from.
	 * @return The JSON document.
	 */
	rapidjson::Document LoadDocument(const std::filesystem::path& file)
	{
		// Open the file in the read mode.
		auto glTF = std::fstream(file, std::ios::in | std::ios::binary | std::ios::ate);

		// Load the JSON data if possible.
		rapidjson::Document document;
		if (glTF.is_open())
		{
			// Get the file size.
			const auto size = glTF.tellg();
			glTF.seekg(0);

			// Create the string and load the data to it.
			std::string fileData;
			fileData.resize(size);

			glTF.read(fileData.data(), size);

			// Parse the loaded data and close the file.
			document.Parse(fileData.c_str());
			glTF.close();
		}

		return document;
	}
}

namespace Xenon
{
	Xenon::MeshStorage MeshStorage::FromFile(Instance& instance, const std::filesystem::path& file)
	{
		const auto extension = file.extension();

		// Validate and check if the file is a glTF file, or if the file exists.
		if (extension != ".gltf" || extension != ".glb" || !std::filesystem::exists(file))
			return MeshStorage(instance);

		// Load the model data.
		const auto modelData = LoadDocument(file);

		return MeshStorage(instance);
	}
}