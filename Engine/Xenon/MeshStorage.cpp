// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "MeshStorage.hpp"
#include "../XenonCore/Logging.hpp"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

#include <fstream>

constexpr const char* Attributes[] = {
	"POSITION",
	"NORMAL",
	"TANGENT",
	"COLOR_0",
	"COLOR_1",
	"COLOR_2",
	"COLOR_3",
	"COLOR_4",
	"COLOR_5",
	"COLOR_6",
	"COLOR_7",
	"TEXCOORD_0",
	"TEXCOORD_1",
	"TEXCOORD_2",
	"TEXCOORD_3",
	"TEXCOORD_4",
	"TEXCOORD_5",
	"TEXCOORD_6",
	"TEXCOORD_7",
	"JOINTS_0",
	"WEIGHTS_0",
};

namespace /* anonymous */
{
	/**
	 * Check if the attribute exists in the primitive and if so, setup the vertex specification for that element.
	 *
	 * @param model The model to get the element information.
	 * @param primitive The primitive containing the attribute information.
	 * @param attribute The attribute name to check.
	 * @param element The vertex element.
	 * @param specification The specification to configure.
	 */
	void ResolvePrimitive(const tinygltf::Model& model, const tinygltf::Primitive& primitive, const std::string& attribute, Xenon::VertexElement element, Xenon::VertexSpecification& specification)
	{
		if (primitive.attributes.contains(attribute))
		{
			const auto index = primitive.attributes.at(attribute);
			const auto& accessor = model.accessors[index];

			// Setup the component type.
			switch (accessor.componentType)
			{
			case TINYGLTF_COMPONENT_TYPE_BYTE:
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
				specification.addElement(element, sizeof(std::byte));
				break;

			case TINYGLTF_COMPONENT_TYPE_SHORT:
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
				specification.addElement(element, sizeof(short));
				break;

			case TINYGLTF_COMPONENT_TYPE_INT:
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
				specification.addElement(element, sizeof(int));
				break;

			case TINYGLTF_COMPONENT_TYPE_FLOAT:
				specification.addElement(element, sizeof(float));
				break;

			case TINYGLTF_COMPONENT_TYPE_DOUBLE:
				specification.addElement(element, sizeof(double));
				break;

			default:
				XENON_LOG_ERROR("Invalid or unsupported vertex element type in the provided model file.");
				break;
			}

			// Setup the data type.
			switch (accessor.type)
			{
			case TINYGLTF_TYPE_VEC2:
			case TINYGLTF_TYPE_VEC3:
			case TINYGLTF_TYPE_VEC4:
			case TINYGLTF_TYPE_MAT2:
			case TINYGLTF_TYPE_MAT3:
			case TINYGLTF_TYPE_MAT4:
			case TINYGLTF_TYPE_SCALAR:
			case TINYGLTF_TYPE_VECTOR:
			case TINYGLTF_TYPE_MATRIX:
				break;

			default:
				XENON_LOG_ERROR("Invalid or unsupported vertex data type in the provided model file.");
				break;
			}

			// Setup the vertex mode.
			switch (primitive.mode)
			{
			case TINYGLTF_MODE_POINTS:
			case TINYGLTF_MODE_LINE:
			case TINYGLTF_MODE_LINE_LOOP:
			case TINYGLTF_MODE_LINE_STRIP:
			case TINYGLTF_MODE_TRIANGLES:
			case TINYGLTF_MODE_TRIANGLE_STRIP:
			case TINYGLTF_MODE_TRIANGLE_FAN:
				break;

			default:
				XENON_LOG_ERROR("Invalid or unsupported vertex mode type in the provided model file.");
				break;
			}
		}
	}
}

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

			// Resolve the vertex specification.
			for (const auto& mesh : model.meshes)
			{
				for (const auto& primitive : mesh.primitives)
				{
					for (std::underlying_type_t<VertexElement> i = 0; i < EnumToInt(VertexElement::Count); i++)
						ResolvePrimitive(model, primitive, Attributes[i], static_cast<VertexElement>(i), storage.m_VertexSpecification);
				}
			}

			// Load the mesh information.
			for (const auto& gltfMesh : model.meshes)
			{
				Mesh mesh;
				mesh.m_Name = gltfMesh.name;

				for (const auto& gltfPrimitive : gltfMesh.primitives)
				{
					const auto& material = model.materials[gltfPrimitive.material];

					if (gltfPrimitive.attributes.contains(Attributes[EnumToInt(VertexElement::Position)]))
					{
						const auto& accessor = model.accessors[gltfPrimitive.attributes.at(Attributes[EnumToInt(VertexElement::Position)])];
						const auto& bufferView = model.bufferViews[accessor.bufferView];
						const auto& buffer = model.buffers[bufferView.buffer];

						auto positionBegin = buffer.data.begin() + accessor.byteOffset + bufferView.byteOffset;
						auto positionEnd = positionBegin + accessor.count;
						const auto stride = accessor.ByteStride(bufferView);

						bufferView.byteStride;
						accessor.componentType;
						accessor.type;
					}
				}
			}
		}

		return storage;
	}
}