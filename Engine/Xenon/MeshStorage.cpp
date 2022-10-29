// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "MeshStorage.hpp"
#include "../XenonCore/Logging.hpp"
#include "../XenonCore/JobSystem.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <fstream>

namespace /* anonymous */
{
	/**
	 * Get the node's information along with it's child nodes.
	 *
	 * @param pScene The scene pointer.
	 * @param pNode The node pointer.
	 * @param specification The vertex specification.
	 * @param vertexCount The variable to store the vertex count.
	 * @param indexCount The variable to store the vertex count.
	 */
	void GetNodeInformation(const aiScene* pScene, const aiNode* pNode, Xenon::VertexSpecification& specification, uint64_t& vertexCount, uint64_t& indexCount)
	{
		// Get the node's mesh information.
		for (uint32_t i = 0; i < pNode->mNumMeshes; i++)
		{
			const auto pMesh = pScene->mMeshes[pNode->mMeshes[i]];
			vertexCount += pMesh->mNumVertices;

			if (pMesh->HasPositions()) specification.addElement(Xenon::VertexElement::Position);
			if (pMesh->HasNormals()) specification.addElement(Xenon::VertexElement::Normal);
			if (pMesh->HasTangentsAndBitangents()) specification.addElement(Xenon::VertexElement::Tangent);

			if (pMesh->HasVertexColors(0)) specification.addElement(Xenon::VertexElement::Color_0);
			if (pMesh->HasVertexColors(1)) specification.addElement(Xenon::VertexElement::Color_1);
			if (pMesh->HasVertexColors(2)) specification.addElement(Xenon::VertexElement::Color_2);
			if (pMesh->HasVertexColors(3)) specification.addElement(Xenon::VertexElement::Color_3);
			if (pMesh->HasVertexColors(4)) specification.addElement(Xenon::VertexElement::Color_4);
			if (pMesh->HasVertexColors(5)) specification.addElement(Xenon::VertexElement::Color_5);
			if (pMesh->HasVertexColors(6)) specification.addElement(Xenon::VertexElement::Color_6);
			if (pMesh->HasVertexColors(7)) specification.addElement(Xenon::VertexElement::Color_7);

			if (pMesh->HasTextureCoords(0)) specification.addElement(Xenon::VertexElement::TextureCoordinate_0);
			if (pMesh->HasTextureCoords(1)) specification.addElement(Xenon::VertexElement::TextureCoordinate_1);
			if (pMesh->HasTextureCoords(2)) specification.addElement(Xenon::VertexElement::TextureCoordinate_2);
			if (pMesh->HasTextureCoords(3)) specification.addElement(Xenon::VertexElement::TextureCoordinate_3);
			if (pMesh->HasTextureCoords(4)) specification.addElement(Xenon::VertexElement::TextureCoordinate_4);
			if (pMesh->HasTextureCoords(5)) specification.addElement(Xenon::VertexElement::TextureCoordinate_5);
			if (pMesh->HasTextureCoords(6)) specification.addElement(Xenon::VertexElement::TextureCoordinate_6);
			if (pMesh->HasTextureCoords(7)) specification.addElement(Xenon::VertexElement::TextureCoordinate_7);
			// if (pMesh->HasBones()) specification.addElement(Xenon::VertexElement::Bone);

			// Get the index count.
			for (uint32_t f = 0; f < pMesh->mNumFaces; f++)
				indexCount += pMesh->mFaces[f].mNumIndices;
		}

		// Get the child nodes' information.
		for (uint32_t i = 0; i < pNode->mNumChildren; i++)
			GetNodeInformation(pScene, pNode->mChildren[i], specification, vertexCount, indexCount);
	}

	/**
	 * Copy the Assimp vector to the destination and increment the pointer by the type size.
	 *
	 * @tparam Type The vector type.
	 * @param vector The vector to copy.
	 * @param pDestination The location to copy to.
	 */
	template<class Type>
	void CopyIncrement(const Type& vector, std::byte*& pDestination)
	{
		Type& destination = *reinterpret_cast<Type*>(pDestination);
		destination = vector;
		pDestination += sizeof(Type);
	}

	/**
	 * Load a node from the scene recursively.
	 *
	 * @param pScene The scene to which the node is attached to.
	 * @param pNode The node to load.
	 * @param pDestination The location to load the data to.
	 * @param indices The vector to store the indices.
	 * @param specification The vertex specification to load the data.
	 * @param meshes The mesh storage.
	 */
	void LoadNode(const aiScene* pScene, const aiNode* pNode, std::byte* pDestination, std::vector<uint32_t>& indices, const Xenon::VertexSpecification& specification, std::vector<Xenon::Mesh>& meshes)
	{
		static auto jobSystem = Xenon::JobSystem(std::thread::hardware_concurrency());

		const auto vertexSize = specification.getSize();

		// Load the meshes of the current node.
		for (uint32_t i = 0; i < pNode->mNumMeshes; i++)
		{
			const auto pMesh = pScene->mMeshes[pNode->mMeshes[i]];

			// Resolve the offsets.
			uint64_t vertexOffset = 0;
			uint64_t indexOffset = 0;

			if (meshes.size() > 0)
			{
				const auto& backMesh = meshes.back();
				vertexOffset = (backMesh.m_VertexCount * vertexSize) + backMesh.m_VertexOffset;
				indexOffset = (backMesh.m_IndexCount * sizeof(uint32_t)) + backMesh.m_IndexOffset;
			}

			// Setup the new mesh.
			auto& mesh = meshes.emplace_back();
			mesh.m_Name = pMesh->mName.C_Str();
			mesh.m_VertexCount = pMesh->mNumVertices;
			mesh.m_VertexOffset = vertexOffset;
			mesh.m_IndexOffset = indexOffset;
			// mesh.m_PrimitiveType = pMesh->mPrimitiveTypes == aiPrimitiveType::aiPrimitiveType_LINE;

			// auto indexIterator = indices.begin();
			// jobSystem.insert([pMesh, vertexSize, pDestination, &specification, indexIterator]() mutable
			// 	{
					// Load the vertex data.
			for (uint32_t j = 0; j < pMesh->mNumVertices; j++)
			{
				// Load the position.
				if (specification.isAvailable(Xenon::VertexElement::Position))
				{
					if (pMesh->HasPositions())
						CopyIncrement(pMesh->mVertices[j], pDestination);

					else
						CopyIncrement(aiVector3D(0.0), pDestination);
				}

				// Load the normal.
				if (specification.isAvailable(Xenon::VertexElement::Normal))
				{
					if (pMesh->HasNormals())
						CopyIncrement(pMesh->mNormals[j], pDestination);

					else
						CopyIncrement(aiVector3D(0.0), pDestination);
				}

				// Load the tangent.
				if (specification.isAvailable(Xenon::VertexElement::Tangent))
				{
					if (pMesh->HasTangentsAndBitangents())
						CopyIncrement(pMesh->mTangents[j], pDestination);

					else
						CopyIncrement(aiVector3D(0.0), pDestination);
				}

				// Load the color values.
				for (uint8_t k = 0; k < 8; k++)
				{
					if (specification.isAvailable(static_cast<Xenon::VertexElement>(EnumToInt(Xenon::VertexElement::Color_0) + k)))
					{
						if (pMesh->HasVertexColors(k))
							CopyIncrement(pMesh->mColors[k][j], pDestination);

						else
							CopyIncrement(aiColor4D(0.0), pDestination);
					}
				}

				// Load the texture coordinate values.
				for (uint8_t k = 0; k < 8; k++)
				{
					if (specification.isAvailable(static_cast<Xenon::VertexElement>(EnumToInt(Xenon::VertexElement::TextureCoordinate_0) + k)))
					{
						if (pMesh->HasTextureCoords(0))
						{
							const auto coordinate = pMesh->mTextureCoords[0][j];
							CopyIncrement(aiVector2D(coordinate.x, coordinate.y), pDestination);
						}

						else
						{
							CopyIncrement(aiVector2D(0.0), pDestination);
						}
					}
				}

				// TODO: Bone stuff.
			}

			// Add the index data.
			for (uint32_t f = 0; f < pMesh->mNumFaces; f++)
			{
				const auto& face = pMesh->mFaces[f];
				for (uint32_t index = 0; index < face.mNumIndices; index++)
					indices.emplace_back(face.mIndices[index]);
			}
			// 	}
			// );

			// Load the index data.
			const auto previousSize = indices.size();
			for (uint32_t f = 0; f < pMesh->mNumFaces; f++)
			{
				const auto& face = pMesh->mFaces[f];
				for (uint32_t index = 0; index < face.mNumIndices; index++)
					indices.emplace_back(face.mIndices[index]);
			}

			mesh.m_IndexCount = indices.size() - previousSize;

			pDestination += mesh.m_VertexCount * vertexSize;
		}

		// Load the children.
		for (uint32_t i = 0; i < pNode->mNumChildren; i++)
			LoadNode(pScene, pNode->mChildren[i], pDestination, indices, specification, meshes);
	}
}

namespace Xenon
{
	Xenon::MeshStorage MeshStorage::FromFile(Instance& instance, const std::filesystem::path& file)
	{
		auto storage = MeshStorage(instance);

		// Load the asset.
		Assimp::Importer importer = {};
		const aiScene* pScene = importer.ReadFile(file.string(),
			aiProcess_CalcTangentSpace |
			aiProcess_JoinIdenticalVertices |
			aiProcess_Triangulate |
			//aiProcess_OptimizeMeshes |
			//aiProcess_OptimizeGraph |
			aiProcess_SortByPType |
			aiProcess_GenUVCoords |
			aiProcess_FlipUVs);

		// Check if the file was loaded.
		if (pScene == nullptr)
		{
			XENON_LOG_FATAL("Failed to load the asset file {}!", file.string());
			return storage;
		}

		// Setup the vertex specification, vertex count and the index count.
		uint64_t vertexCount = 0;
		uint64_t indexCount = 0;
		GetNodeInformation(pScene, pScene->mRootNode, storage.m_VertexSpecification, vertexCount, indexCount);

		// Setup the staging buffer to load the vertex data to.
		const auto vertexBufferSize = vertexCount * storage.m_VertexSpecification.getSize();

		{
			// Test image creation.
			const auto pImage = instance.getFactory()->createImage(instance.getBackendDevice(), { .m_Width = 1280, .m_Height = 720, .m_Format = Backend::DataFormat::R8G8B8A8_SRGB });
			return storage;
		}

		// Load the nodes recursively.
		auto pLocalBuffer = std::make_unique<std::byte[]>(vertexBufferSize);
		auto localPointer = pLocalBuffer.get();

		std::vector<uint32_t> localIndexBuffer;
		localIndexBuffer.reserve(indexCount);
		LoadNode(pScene, pScene->mRootNode, localPointer, localIndexBuffer, storage.m_VertexSpecification, storage.m_Meshes);

		// Setup the buffers and load the data.
		auto pVertexStagingBuffer = instance.getFactory()->createBuffer(instance.getBackendDevice(), vertexBufferSize, Backend::BufferType::Staging);
		pVertexStagingBuffer->write(pLocalBuffer.get(), vertexBufferSize);

		const auto indexBufferSize = localIndexBuffer.size() * sizeof(uint32_t);
		auto pIndexStagingBuffer = instance.getFactory()->createBuffer(instance.getBackendDevice(), indexBufferSize, Backend::BufferType::Staging);
		pIndexStagingBuffer->write(reinterpret_cast<const std::byte*>(localIndexBuffer.data()), indexBufferSize);

		// Create the actual final buffers.
		storage.m_pVertexBuffer = instance.getFactory()->createBuffer(instance.getBackendDevice(), vertexBufferSize, Backend::BufferType::Vertex);
		storage.m_pIndexBuffer = instance.getFactory()->createBuffer(instance.getBackendDevice(), indexBufferSize, Backend::BufferType::Index);

		// Load the data to them.
		{
			auto pCommandRecorder = instance.getFactory()->createCommandRecorder(instance.getBackendDevice(), Backend::CommandRecorderUsage::Transfer);
			pCommandRecorder->begin();

			pCommandRecorder->copyBuffer(pVertexStagingBuffer.get(), 0, storage.m_pVertexBuffer.get(), 0, vertexBufferSize);
			pCommandRecorder->copyBuffer(pIndexStagingBuffer.get(), 0, storage.m_pIndexBuffer.get(), 0, indexBufferSize);

			pCommandRecorder->end();
			pCommandRecorder->submit();
			pCommandRecorder->wait();
		}

		return storage;
	}
}