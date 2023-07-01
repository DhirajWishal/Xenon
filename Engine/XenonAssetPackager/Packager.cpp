// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Packager.hpp"
#include "../XenonCore/Common.hpp"

#include <nlohmann/json.hpp>

#include <iostream>
#include <fstream>

using JsonDocument = nlohmann::json;

namespace Xenon
{
	Packager::Packager(const std::filesystem::path& inputFile, const std::filesystem::path& outputFile)
		: m_InputFile(inputFile)
		, m_OutputFile(outputFile)
	{
	}

	uint32_t Packager::package() const
	{
		// Check if the input file exists.
		if (!std::filesystem::exists(m_InputFile))
		{
			std::cout << "The input file does not exist!" << std::endl;
			return -1;
		}

		// Load the input file.
		std::ifstream inputFile(m_InputFile);
		const auto inputData = JsonDocument::parse(inputFile);
		inputFile.close();

		// Load all the data to a new json document.
		JsonDocument loadedData;
		for (auto itr = inputData.begin(); itr != inputData.end(); ++itr)
		{
			const auto& jsonData = *itr;
			const auto& key = itr.key();

			// Check if we need to load anything.
			if (jsonData.is_object() && jsonData.contains("file") && jsonData.contains("type"))
			{
				auto& object = loadedData[key];
				object["type"] = jsonData["type"];
				object["bytes"] = loadFileData(std::string(jsonData["file"]));
			}
			else
			{
				loadedData[key] = jsonData;
			}
		}

		// Compress and write everything to the output file.
		const auto output = JsonDocument::to_cbor(loadedData);
		std::ofstream outputFile(m_OutputFile, std::ios::out | std::ios::binary);
		if (outputFile.is_open())
		{
			outputFile.write(XENON_BIT_CAST(const char*, output.data()), output.size());
		}
		else
		{
			std::cout << "Failed to create and write the output file!" << std::endl;

			outputFile.close();
			return -2;
		}

		outputFile.close();

		// Inform the user that the operation was successful and exit.
		std::cout << "Successfully created the Xenon package file: " << m_OutputFile << std::endl;
		return 0;
	}

	std::vector<std::byte> Packager::loadFileData(const std::filesystem::path& file) const
	{
		std::vector<std::byte> bytes;

		std::ifstream inputFile(file, std::ios::in | std::ios::binary | std::ios::ate);
		if (inputFile.is_open())
		{
			const auto size = inputFile.tellg();
			inputFile.seekg(0);

			bytes.resize(size);
			inputFile.read(XENON_BIT_CAST(char*, bytes.data()), size);
		}
		else
		{
			std::cout << "Failed to load file: " << file << std::endl;
		}

		inputFile.close();
		return bytes;
	}
}