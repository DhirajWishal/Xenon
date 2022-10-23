// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "MeshStorage.hpp"
#include "../XenonCore/Logging.hpp"

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
			std::vector<std::unique_ptr<Backend::Buffer>> pStagingBuffers;
			for (const auto& buffer : model.buffers)
			{
				vertexSize += buffer.data.size();

				auto& pBuffer = pStagingBuffers.emplace_back(instance.getFactory()->createBuffer(instance.getBackendDevice(), buffer.data.size(), Backend::BufferType::Staging));
				pBuffer->write(reinterpret_cast<const std::byte*>(buffer.data.data()), buffer.data.size());
			}

			// Create the vertex buffer.
			storage.m_pVertexBuffer = instance.getFactory()->createBuffer(instance.getBackendDevice(), vertexSize, Backend::BufferType::Vertex);

			// Copy the data to the vertex buffer.
			{
				auto pCommandRecorder = instance.getFactory()->createCommandRecorder(instance.getBackendDevice(), Backend::CommandRecorderUsage::Transfer);
				pCommandRecorder->begin();

				uint64_t offset = 0;
				for (auto& pBuffer : pStagingBuffers)
				{
					pCommandRecorder->copyBuffer(pBuffer.get(), 0, storage.m_pVertexBuffer.get(), offset, pBuffer->getSize());
					offset += pBuffer->getSize();
				}

				pCommandRecorder->end();
				pCommandRecorder->submit();
				pCommandRecorder->wait();
			}

			// Clear the staging buffers to save memory.
			pStagingBuffers.clear();

			const auto bufferView = storage.m_pVertexBuffer->read();
			HexDump(bufferView.begin(), bufferView.end());

			// for (size_t i = 0; i < model.bufferViews.size(); ++i) {
			// 	const auto& bufferView = model.bufferViews[i];
			// 	if (bufferView.target == 0)
			// 		continue;
			// 
			// 	const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
			// 
			// }
		}

		return storage;
	}
}