// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonBackend/Pipeline.hpp"

namespace Xenon
{
	/**
	 * Default cache handler class.
	 * This class is used to save and load pipeline cache.
	 */
	class DefaultCacheHandler final : public Backend::PipelineCacheHandler
	{
	public:
		/**
		 * Load the cache data from the store.
		 *
		 * @param hash The internal hash used to identify unique pipelines.
		 * @return The pipeline cache.
		 */
		[[nodiscard]] std::vector<std::byte> load(uint64_t hash) override;

		/**
		 * Store the cache data generated from the backend.
		 *
		 * @param hash The internal hash used to identify unique pipelines. It's the best to store cache in a way that it can be accessed using the hash.
		 * @param bytes The bytes to store.
		 */
		void store(uint64_t hash, const std::vector<std::byte>& bytes) override;
	};
}