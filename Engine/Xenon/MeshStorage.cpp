// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "MeshStorage.hpp"
#include "../XenonCore/Logging.hpp"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

#include <latch>
#include <fstream>

constexpr const char* g_Attributes[] = {
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

namespace Materials
{
	/**
	 * PBR Metallic Roughness Material.
	 */
	struct PBRMetallicRoughnessMaterial final : public Xenon::MaterialBlob
	{
		std::unique_ptr<Xenon::Backend::Image> m_pImage = nullptr;
		std::unique_ptr<Xenon::Backend::ImageView> m_pImageView = nullptr;
		std::unique_ptr<Xenon::Backend::ImageSampler> m_pSampler = nullptr;
	};
}

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
	 * @return The size of the vertex buffer to store all the vertices.
	 */
	uint64_t ResolvePrimitive(
		const tinygltf::Model& model,
		const tinygltf::Primitive& primitive,
		const std::string& attribute,
		Xenon::Backend::InputElement element,
		Xenon::Backend::VertexSpecification& specification)
	{
		if (!primitive.attributes.contains(attribute))
			return 0;

		const auto index = primitive.attributes.at(attribute);
		const auto& accessor = model.accessors[index];
		const auto& bufferView = model.bufferViews[accessor.bufferView];

		// Setup the component type.
		switch (accessor.componentType)
		{
		case TINYGLTF_COMPONENT_TYPE_BYTE:
			specification.addElement(element, Xenon::Backend::ComponentDataType::Uint8);
			break;

		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
			specification.addElement(element, Xenon::Backend::ComponentDataType::Int8);
			break;

		case TINYGLTF_COMPONENT_TYPE_SHORT:
			specification.addElement(element, Xenon::Backend::ComponentDataType::Int16);
			break;

		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
			specification.addElement(element, Xenon::Backend::ComponentDataType::Uint16);
			break;

		case TINYGLTF_COMPONENT_TYPE_INT:
			specification.addElement(element, Xenon::Backend::ComponentDataType::Int32);
			break;

		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
			specification.addElement(element, Xenon::Backend::ComponentDataType::Uint32);
			break;

		case TINYGLTF_COMPONENT_TYPE_FLOAT:
			specification.addElement(element, Xenon::Backend::ComponentDataType::Float);
			break;

		case TINYGLTF_COMPONENT_TYPE_DOUBLE:
			specification.addElement(element, Xenon::Backend::ComponentDataType::Double);
			break;

		default:
			XENON_LOG_ERROR("Invalid or unsupported vertex element type in the provided model file.");
			break;
		}

		return accessor.ByteStride(bufferView) * accessor.count;
	}

	/**
	 * Attribute view structure.
	 */
	struct AttributeView final
	{
		using Iterator = std::vector<unsigned char>::const_iterator;

		Iterator m_Begin;
		Iterator m_End;

		uint64_t m_Size = 0;
		uint32_t m_Stride = 0;

		Xenon::Backend::InputElement m_Element = Xenon::Backend::InputElement::VertexPosition;
		Xenon::Backend::AttributeDataType m_DataType = Xenon::Backend::AttributeDataType::Vec3;
		Xenon::PrimitiveMode m_PrimitiveMode = Xenon::PrimitiveMode::Triangles;
	};

	/**
	 * Get an attribute's buffer view.
	 *
	 * @param model The model in which the data are stored.
	 * @param primitive The primitive to access.
	 * @param element The vertex element to get the view of.
	 * @return The tuple of iterators and sizes containing the begin and end iterators and the byte stride.
	 */
	decltype(auto) GetAttributeView(const tinygltf::Model& model, const tinygltf::Primitive& primitive, Xenon::Backend::InputElement element)
	{
		AttributeView view;
		view.m_Element = element;

		if (!primitive.attributes.contains(g_Attributes[Xenon::EnumToInt(element)]))
			return view;

		const auto& accessor = model.accessors[primitive.attributes.at(g_Attributes[Xenon::EnumToInt(element)])];
		const auto& bufferView = model.bufferViews[accessor.bufferView];
		const auto& buffer = model.buffers[bufferView.buffer];

		view.m_Stride = accessor.ByteStride(bufferView);
		view.m_Begin = buffer.data.begin() + accessor.byteOffset + bufferView.byteOffset;
		view.m_End = view.m_Begin + (accessor.count * view.m_Stride);

		view.m_Size = std::distance(view.m_Begin, view.m_End);

		// Setup the data type.
		switch (accessor.type)
		{
		case TINYGLTF_TYPE_VEC2:
			view.m_DataType = Xenon::Backend::AttributeDataType::Vec2;
			break;

		case TINYGLTF_TYPE_VEC3:
			view.m_DataType = Xenon::Backend::AttributeDataType::Vec3;
			break;

		case TINYGLTF_TYPE_VEC4:
			view.m_DataType = Xenon::Backend::AttributeDataType::Vec4;
			break;

		case TINYGLTF_TYPE_MAT2:
			view.m_DataType = Xenon::Backend::AttributeDataType::Mat2;
			break;

		case TINYGLTF_TYPE_MAT3:
			view.m_DataType = Xenon::Backend::AttributeDataType::Mat3;
			break;

		case TINYGLTF_TYPE_MAT4:
			view.m_DataType = Xenon::Backend::AttributeDataType::Mat4;
			break;

		case TINYGLTF_TYPE_SCALAR:
			view.m_DataType = Xenon::Backend::AttributeDataType::Scalar;
			break;

		case TINYGLTF_TYPE_VECTOR:
			view.m_DataType = Xenon::Backend::AttributeDataType::Vec3;
			break;

		case TINYGLTF_TYPE_MATRIX:
			view.m_DataType = Xenon::Backend::AttributeDataType::Mat4;
			break;

		default:
			XENON_LOG_ERROR("Invalid or unsupported vertex data type in the provided model file. Defaulting to vector 3.");
			view.m_DataType = Xenon::Backend::AttributeDataType::Vec3;
			break;
		}

		// Setup the vertex mode.
		switch (primitive.mode)
		{
		case TINYGLTF_MODE_POINTS:
			view.m_PrimitiveMode = Xenon::PrimitiveMode::Points;
			break;

		case TINYGLTF_MODE_LINE:
			view.m_PrimitiveMode = Xenon::PrimitiveMode::Line;
			break;

		case TINYGLTF_MODE_LINE_LOOP:
			view.m_PrimitiveMode = Xenon::PrimitiveMode::LineLoop;
			break;

		case TINYGLTF_MODE_LINE_STRIP:
			view.m_PrimitiveMode = Xenon::PrimitiveMode::LineStrip;
			break;

		case TINYGLTF_MODE_TRIANGLES:
			view.m_PrimitiveMode = Xenon::PrimitiveMode::Triangles;
			break;

		case TINYGLTF_MODE_TRIANGLE_STRIP:
			view.m_PrimitiveMode = Xenon::PrimitiveMode::TriangleStrip;
			break;

		case TINYGLTF_MODE_TRIANGLE_FAN:
			view.m_PrimitiveMode = Xenon::PrimitiveMode::TriangleFan;
			break;

		default:
			XENON_LOG_ERROR("Invalid or unsupported vertex mode type in the provided model file. Defaulting to triangle.");
			view.m_PrimitiveMode = Xenon::PrimitiveMode::Triangles;
			break;
		}

		return view;
	}

	/**
	 * Load a sub-mesh from a primitive.
	 *
	 * @param subMesh The sub-mesh to load the data to.
	 * @param specification The vertex specification.
	 * @param model The glTF model.
	 * @param primitive The glTF primitive.
	 * @param vertexBegin The vertex begin iterator to load the data to.
	 * @param indexBegin The index begin iterator to load the data to.
	 */
	void LoadSubMesh(
		Xenon::SubMesh& subMesh,
		const Xenon::Backend::VertexSpecification& specification,
		const tinygltf::Model& model,
		const tinygltf::Primitive& primitive,
		std::vector<unsigned char>::iterator vertexBegin,
		std::vector<unsigned char>::iterator indexBegin)
	{
		// Setup the primitive mode.
		switch (primitive.mode)
		{
		case TINYGLTF_MODE_POINTS:
			subMesh.m_Mode = Xenon::PrimitiveMode::Points;
			break;

		case TINYGLTF_MODE_LINE:
			subMesh.m_Mode = Xenon::PrimitiveMode::Line;
			break;

		case TINYGLTF_MODE_LINE_LOOP:
			subMesh.m_Mode = Xenon::PrimitiveMode::LineLoop;
			break;

		case TINYGLTF_MODE_LINE_STRIP:
			subMesh.m_Mode = Xenon::PrimitiveMode::LineStrip;
			break;

		case TINYGLTF_MODE_TRIANGLES:
			subMesh.m_Mode = Xenon::PrimitiveMode::Triangles;
			break;

		case TINYGLTF_MODE_TRIANGLE_STRIP:
			subMesh.m_Mode = Xenon::PrimitiveMode::TriangleStrip;
			break;

		case TINYGLTF_MODE_TRIANGLE_FAN:
			subMesh.m_Mode = Xenon::PrimitiveMode::TriangleFan;
			break;

		default:
			XENON_LOG_ERROR("Invalid or unsupported vertex mode type in the provided model file. Defaulting to triangle.");
			subMesh.m_Mode = Xenon::PrimitiveMode::Triangles;
			break;
		}

		uint64_t vertexStride = 0;
		uint64_t vertexBufferSize = 0;

		// Get the vertex information.
		std::vector<AttributeView> attributes;
		for (auto i = Xenon::EnumToInt(Xenon::Backend::InputElement::VertexPosition); i < Xenon::EnumToInt(Xenon::Backend::InputElement::VertexElementCount); i++)
		{
			if (specification.isAvailable(static_cast<Xenon::Backend::InputElement>(i)))
			{
				const auto& view = attributes.emplace_back(GetAttributeView(model, primitive, static_cast<Xenon::Backend::InputElement>(i)));
				vertexStride += view.m_Stride;
				vertexBufferSize += view.m_Size;
			}
		}

		// Load the vertex data to the buffer.
		subMesh.m_VertexCount = vertexBufferSize / vertexStride;
		for (uint64_t i = 0; i < subMesh.m_VertexCount; i++)
		{
			for (auto& attribute : attributes)
			{
				if (attribute.m_Begin != attribute.m_End)
				{
					std::copy_n(attribute.m_Begin, attribute.m_Stride, vertexBegin);
					attribute.m_Begin += attribute.m_Stride;
				}

				vertexBegin += attribute.m_Stride;
			}
		}

		// Load the index buffer data.
		if (primitive.indices >= 0)
		{
			const auto& accessor = model.accessors.at(primitive.indices);
			const auto& bufferView = model.bufferViews.at(accessor.bufferView);
			const auto& buffer = model.buffers.at(bufferView.buffer);

			const auto stride = accessor.ByteStride(bufferView);
			const auto start = accessor.byteOffset + bufferView.byteOffset;
			const auto end = start + accessor.count * stride;

			subMesh.m_IndexCount = accessor.count;
			subMesh.m_IndexSize = static_cast<uint8_t>(stride);
			subMesh.m_IndexOffset /= subMesh.m_IndexSize;

			std::copy(buffer.data.begin() + start, buffer.data.begin() + end, indexBegin);
		}
	}

	/**
	 * Load a node from the model.
	 *
	 * @param instance The instance reference.
	 * @param model The model to load from.
	 * @param node The node to load.
	 * @param storage The mesh storage.
	 * @param vertices The vertex storage.
	 * @param vertexItr The vertex storage iterator.
	 * @param indices The index storage.
	 * @param indexItr The index storage iterator.
	 * @param synchronization The synchronization latch.
	 */
	void LoadNode(
		Xenon::Instance& instance,
		const tinygltf::Model& model,
		const tinygltf::Node& node,
		Xenon::MeshStorage& storage,
		std::vector<unsigned char>& vertices,
		std::vector<unsigned char>::iterator& vertexItr,
		std::vector<unsigned char>& indices,
		std::vector<unsigned char>::iterator& indexItr,
		std::latch& synchronization)
	{
		static auto workers = Xenon::JobSystem(std::thread::hardware_concurrency() - 1);	// Keep one thread free for other purposes.
		static std::unordered_map<uint64_t, Materials::PBRMetallicRoughnessMaterial> materialMap;

		// Get the mesh and initialize everything.
		const auto& gltfMesh = model.meshes[node.mesh];
		auto& mesh = storage.getMeshes().emplace_back();
		mesh.m_Name = gltfMesh.name;
		mesh.m_SubMeshes.reserve(gltfMesh.primitives.size());

		// Load the sub-mesh information.
		for (const auto& gltfPrimitive : gltfMesh.primitives)
		{
			// Create the primitive.
			auto& subMesh = mesh.m_SubMeshes.emplace_back();
			subMesh.m_VertexOffset = std::distance(vertices.begin(), vertexItr);
			subMesh.m_IndexOffset = std::distance(indices.begin(), indexItr);

			// Insert the job.
			workers.insert([&subMesh, &model, &storage, &gltfPrimitive, vertexItr, indexItr, &synchronization]
				{
					LoadSubMesh(subMesh, storage.getVertexSpecification(), model, gltfPrimitive, vertexItr, indexItr);
			synchronization.count_down();
				}
			);

			// Get the next available vertex begin position.
			for (auto i = Xenon::EnumToInt(Xenon::Backend::InputElement::VertexPosition); i < Xenon::EnumToInt(Xenon::Backend::InputElement::VertexElementCount); i++)
			{
				if (storage.getVertexSpecification().isAvailable(static_cast<Xenon::Backend::InputElement>(i)) && gltfPrimitive.attributes.contains(g_Attributes[i]))
				{
					const auto& accessor = model.accessors[gltfPrimitive.attributes.at(g_Attributes[i])];
					const auto& bufferView = model.bufferViews[accessor.bufferView];

					vertexItr += accessor.ByteStride(bufferView) * accessor.count;
				}
			}

			// Get the next available index begin position.
			if (gltfPrimitive.indices >= 0)
			{
				const auto& accessor = model.accessors.at(gltfPrimitive.indices);
				const auto& bufferView = model.bufferViews.at(accessor.bufferView);

				indexItr += accessor.ByteStride(bufferView) * accessor.count;
			}

			// Load materials.
			auto material = model.materials[gltfPrimitive.material];
			if (material.pbrMetallicRoughness.baseColorTexture.index >= 0)
			{
				auto texture = model.textures[material.pbrMetallicRoughness.baseColorTexture.index];
				auto image = model.images[texture.source];
				auto sampler = model.samplers[texture.sampler];

				auto pMaterial = std::make_unique<Materials::PBRMetallicRoughnessMaterial>();

				const auto imageHash = Xenon::GenerateHash(Xenon::ToBytes(image.image.data()), image.image.size());
				if (materialMap.contains(imageHash))
				{
					const auto& loadedMaterial = materialMap[imageHash];

					// Setup the descriptor.
					std::vector<Xenon::Backend::DescriptorBindingInfo> bindingInfos;
					auto& info = bindingInfos.emplace_back();
					info.m_ApplicableShaders = Xenon::Backend::ShaderType::Fragment;
					info.m_Type = Xenon::Backend::ResourceType::CombinedImageSampler;

					pMaterial->m_pDescriptor = instance.getFactory()->createDescriptor(instance.getBackendDevice(), bindingInfos, Xenon::Backend::DescriptorType::Material);

					// Attach the resources.
					pMaterial->m_pDescriptor->attach(0, loadedMaterial.m_pImage.get(), loadedMaterial.m_pImageView.get(), loadedMaterial.m_pSampler.get(), Xenon::Backend::ImageUsage::Graphics);

					// Set the material.
					subMesh.m_pMaterial = std::move(pMaterial);
				}
				else
				{
					auto& loadedMaterial = materialMap[imageHash];

					// Setup the image.
					Xenon::Backend::ImageSpecification imageSpecification = {};
					imageSpecification.m_Width = image.width;
					imageSpecification.m_Height = image.height;
					imageSpecification.m_Format = Xenon::Backend::DataFormat::R8G8B8A8_SRGB;

					loadedMaterial.m_pImage = instance.getFactory()->createImage(instance.getBackendDevice(), imageSpecification);

					// Copy the image data to the image.
					{
						const auto copySize = image.width * image.height * image.component;
						auto pStagingBuffer = instance.getFactory()->createBuffer(instance.getBackendDevice(), copySize, Xenon::Backend::BufferType::Staging);

						pStagingBuffer->write(Xenon::ToBytes(image.image.data()), copySize);
						loadedMaterial.m_pImage->copyFrom(pStagingBuffer.get());
					}

					// Setup image view.
					Xenon::Backend::ImageViewSpecification imageViewSpecification = {};
					loadedMaterial.m_pImageView = instance.getFactory()->createImageView(instance.getBackendDevice(), loadedMaterial.m_pImage.get(), imageViewSpecification);

					// Setup image sampler.
					Xenon::Backend::ImageSamplerSpecification imageSamplerSpecification = {};
					loadedMaterial.m_pSampler = instance.getFactory()->createImageSampler(instance.getBackendDevice(), imageSamplerSpecification);

					// Setup the descriptor.
					std::vector<Xenon::Backend::DescriptorBindingInfo> bindingInfos;
					auto& info = bindingInfos.emplace_back();
					info.m_ApplicableShaders = Xenon::Backend::ShaderType::Fragment;
					info.m_Type = Xenon::Backend::ResourceType::CombinedImageSampler;

					pMaterial->m_pDescriptor = instance.getFactory()->createDescriptor(instance.getBackendDevice(), bindingInfos, Xenon::Backend::DescriptorType::Material);

					// Attach the resources.
					pMaterial->m_pDescriptor->attach(0, loadedMaterial.m_pImage.get(), loadedMaterial.m_pImageView.get(), loadedMaterial.m_pSampler.get(), Xenon::Backend::ImageUsage::Graphics);

					// Set the material.
					subMesh.m_pMaterial = std::move(pMaterial);
				}
			}
			else
			{
				std::vector<Xenon::Backend::DescriptorBindingInfo> bindingInfos;
				auto& info = bindingInfos.emplace_back();
				info.m_ApplicableShaders = Xenon::Backend::ShaderType::Fragment;
				info.m_Type = Xenon::Backend::ResourceType::CombinedImageSampler;

				subMesh.m_pMaterial->m_pDescriptor = instance.getFactory()->createDescriptor(instance.getBackendDevice(), bindingInfos, Xenon::Backend::DescriptorType::Material);

				// Attach the resources.
				subMesh.m_pMaterial->m_pDescriptor->attach(0, instance.getDefaultImage(), instance.getDefaultImageView(), instance.getDefaultSampler(), Xenon::Backend::ImageUsage::Graphics);
			}
		}

		// Load the children.
		for (const auto child : node.children)
			LoadNode(instance, model, model.nodes[child], storage, vertices, vertexItr, indices, indexItr, synchronization);
	}
}

namespace Xenon
{
	Xenon::MeshStorage MeshStorage::FromFile(Instance& instance, const std::filesystem::path& file)
	{
		MeshStorage storage;

		// Load the model data.
		tinygltf::Model model;
		std::string errorString;
		std::string warningString;

		// Try and load the model.
		if (tinygltf::TinyGLTF loader; !loader.LoadASCIIFromFile(&model, &errorString, &warningString, file.string()))
		{
			// Show the error if there are any.
			if (!errorString.empty())
				XENON_LOG_ERROR("glTF loading error: {}", errorString);

			// Show the warning if there are any.
			if (!warningString.empty())
				XENON_LOG_WARNING("glTF loading warning: {}", warningString);

			return storage;
		}

		// Show the warning if there are any.
		if (!warningString.empty())
			XENON_LOG_WARNING("glTF loading warning: {}", warningString);

		// Resolve the vertex specification and other information.
		uint64_t vertexBufferSize = 0;
		uint64_t indexBufferSize = 0;

		uint32_t workerSubmissions = 0;
		for (const auto& mesh : model.meshes)
		{
			workerSubmissions += mesh.primitives.size();
			for (const auto& primitive : mesh.primitives)
			{
				// Get the vertex information.
				for (auto i = EnumToInt(Backend::InputElement::VertexPosition); i < EnumToInt(Backend::InputElement::VertexElementCount); i++)
					vertexBufferSize += ResolvePrimitive(model, primitive, g_Attributes[i], static_cast<Backend::InputElement>(i), storage.m_VertexSpecification);

				// Get the index buffer size.
				if (primitive.indices >= 0)
				{
					const auto& accessor = model.accessors.at(primitive.indices);
					const auto& bufferView = model.bufferViews.at(accessor.bufferView);

					indexBufferSize += accessor.count * accessor.ByteStride(bufferView);
				}
			}
		}

		// Load the mesh information.
		auto vertices = std::vector<unsigned char>(vertexBufferSize);
		auto vertexItr = vertices.begin();

		auto indices = std::vector<unsigned char>(indexBufferSize);
		auto indexItr = indices.begin();

		// Load the default scene.
		storage.m_Meshes.reserve(model.meshes.size());
		auto synchronization = std::latch(workerSubmissions);

		const auto& scene = model.scenes[model.defaultScene];
		LoadNode(instance, model, model.nodes[scene.nodes.front()], storage, vertices, vertexItr, indices, indexItr, synchronization);

		// Wait till all the sub-meshes are loaded.
		synchronization.wait();

		// Load the vertex data and clear the buffer.
		storage.m_pVertexBuffer = instance.getFactory()->createBuffer(instance.getBackendDevice(), vertexBufferSize, Backend::BufferType::Vertex);
		storage.m_pVertexBuffer->write(ToBytes(vertices.data()), vertexBufferSize);
		vertices.clear();

		// Load the index data and clear the buffer.
		storage.m_pIndexBuffer = instance.getFactory()->createBuffer(instance.getBackendDevice(), indexBufferSize, Backend::BufferType::Index);
		storage.m_pIndexBuffer->write(ToBytes(indices.data()), indexBufferSize);
		indices.clear();

		return storage;
	}
}
