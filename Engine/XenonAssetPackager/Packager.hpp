// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonCore/Common.hpp"

#include <filesystem>
#include <vector>

namespace Xenon
{
	/**
	 * Packager class.
	 * This class reads all the information from the input JSON document and packs them all into CBOR format and saves them to the
	 * output file.
	 *
	 * The input data format:
	 * {
	 *		"entry1": {
	 *			"file": "something.txt",
	 *			"type": "bytes"
	 *		},
	 *		"entry2": 100,
	 *		"entry3": {
	 *			"x": "something",
	 *			"y": 200
	 *		}
	 * }
	 *
	 * The output format:
	 * {
	 *		"entry1": {
	 *			"bytes": [...],
	 *			"type": "bytes"
	 *		},
	 *		"entry2": 100,
	 *		"entry3": {
	 *			"x": "something",
	 *			"y": 200
	 *		}
	 * }
	 */
	class Packager final
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param inputFile The input file.
		 * @param outputFile The output file.
		 */
		explicit Packager(const std::filesystem::path& inputFile, const std::filesystem::path& outputFile);

		/**
		 * Package everything to CBOR.
		 *
		 * @return The return status.
		 */
		XENON_NODISCARD uint32_t package() const;

	private:
		/**
		 * Load the file data.
		 *
		 * @param file The file to load the data from.
		 * @return The loaded bytes.
		 */
		XENON_NODISCARD std::vector<std::byte> loadFileData(const std::filesystem::path& file) const;

	private:
		std::filesystem::path m_InputFile;
		std::filesystem::path m_OutputFile;
	};
}