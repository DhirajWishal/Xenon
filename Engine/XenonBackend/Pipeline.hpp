// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.hpp"
#include "Descriptor.hpp"

namespace Xenon
{
	namespace Backend
	{
		/**
		 * Pipeline cache handler class.
		 * This class specifies how to handle the pipeline cache of a pipeline.
		 */
		class PipelineCacheHandler
		{
		public:
			/**
			 * Default constructor.
			 */
			PipelineCacheHandler() = default;

			/**
			 * Default virtual destructor.
			 */
			virtual ~PipelineCacheHandler() = default;

			/**
			 * Load the cache data from the store.
			 *
			 * @param hash The internal hash used to identify unique pipelines.
			 * @return The pipeline cache.
			 */
			[[nodiscard]] virtual std::vector<std::byte> load(uint64_t hash) = 0;

			/**
			 * Store the cache data generated from the backend.
			 *
			 * @param hash The internal hash used to identify unique pipelines. It's the best to store cache in a way that it can be accessed using the hash.
			 * @param bytes The bytes to store.
			 */
			virtual void store(uint64_t hash, const std::vector<std::byte>& bytes) = 0;
		};

		/**
		 * Pipeline class.
		 * This is the base class for all the pipelines in the engine.
		 */
		class Pipeline : public BackendObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device pointer.
			 * @param pCacheHandler The cache handler pointer. This can be null in which case the pipeline creation might get slow.
			 */
			explicit Pipeline([[maybe_unused]] const Device* pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler) : m_pCacheHandler(std::move(pCacheHandler)) {}

			/**
			 * Create a new descriptor.
			 *
			 * @param type The descriptor type to create.
			 * @return The descriptor pointer. It will return nullptr if the descriptor type is not present in the pipeline.
			 */
			[[nodiscard]] virtual std::unique_ptr<Descriptor> createDescriptor(DescriptorType type) = 0;

		protected:
			std::unique_ptr<PipelineCacheHandler> m_pCacheHandler = nullptr;
		};
	}
}