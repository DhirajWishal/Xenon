// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "MeshStorage.hpp"

#include <tiny_gltf.h>
#include <fstream>

namespace Xenon
{
	Xenon::MeshStorage MeshStorage::FromFile(Instance& instance, const std::filesystem::path& file)
	{
		const auto extension = file.extension();

		// Validate and check if the file is a glTF file, or if the file exists.
		if (extension != ".gltf" || extension != ".glb" || !std::filesystem::exists(file))
			return MeshStorage(instance);

		// Load the model data.
		tinygltf::Model model;
		std::string errorString;
		std::string warningString;

		tinygltf::TinyGLTF loader;
		if (loader.LoadASCIIFromFile(&model, &errorString, &warningString, file.string()))
		{

		}

		return MeshStorage(instance);
	}
}