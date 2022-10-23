// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "MeshStorage.hpp"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

#include <fstream>

namespace Xenon
{
	Xenon::MeshStorage MeshStorage::FromFile(Instance& instance, const std::filesystem::path& file)
	{
		const auto extension = file.extension();
		auto storage = MeshStorage(instance);

		// Load the model data.
		tinygltf::Model model;
		std::string errorString;
		std::string warningString;

		tinygltf::TinyGLTF loader;
		if (loader.LoadASCIIFromFile(&model, &errorString, &warningString, file.string()))
		{
			// Get the vertex buffer size.
			uint64_t vertexSize = 0;
			std::vector<std::unique_ptr<Backend::StagingBuffer>> pStagingBuffers;
			for (const auto& buffer : model.buffers)
			{
				vertexSize += buffer.data.size();

				auto& pBuffer = pStagingBuffers.emplace_back(instance.getFactory()->createStagingBuffer(instance.getBackendDevice(), buffer.data.size()));
				pBuffer->write(reinterpret_cast<const std::byte*>(buffer.data.data()), buffer.data.size());
			}

			// Create the vertex buffer.
			storage.m_pVertexBuffer = instance.getFactory()->createVertexBuffer(instance.getBackendDevice(), vertexSize, sizeof(uint8_t));

			// Load the vertex buffer data.
			uint64_t offset = 0;
			for (const auto& buffer : pStagingBuffers)
			{
				storage.m_pVertexBuffer->copy(buffer.get(), buffer->getSize(), 0, offset);
				offset += buffer->getSize();
			}

			for (size_t i = 0; i < model.bufferViews.size(); ++i) {
				const auto& bufferView = model.bufferViews[i];
				if (bufferView.target == 0)
					continue;

				const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

			}
		}

		return storage;
	}
}