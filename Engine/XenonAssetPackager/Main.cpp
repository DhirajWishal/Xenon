// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Packager.hpp"

#include <iostream>

/**
 * Usage: XenonAssetPackager [inputFile.xpkg] [outputFile.bin]
 *
 * The "inputFile.xpkg" is a JSON document which describes the data that needs to be packaged.
 * The "outputFile.bin" is a binary file which contains all the packed data. This can be extracted using the engine.
 */

void PrintHelp()
{
	std::cout << "Xenon Asset Packager v1.0" << std::endl;
	std::cout << "The Xenon asset packager application can be used to package assets to an engine friendly asset format." << std::endl;
	std::cout << std::endl;
	std::cout << "Usage: XenonAssetPackager [inputFile.xpkg] [outputFile.bin]" << std::endl;
	std::cout << std::endl;
	std::cout << "[inputFile.xpkg]  - This is a JSON document which describes the data that needs to be packaged." << std::endl;
	std::cout << "[outputFile.xpkg] - This is a binary file which contains all the packed data. This can be extracted using the engine." << std::endl;
}

int main(int argc, char* argv[])
{
	// Validate the argument count.
	if (argc < 3)
	{
		PrintHelp();
		return -1;
	}

	// Let's package it!
	auto packager = Xenon::Packager(argv[1], argv[2]);
	return packager.package();
}