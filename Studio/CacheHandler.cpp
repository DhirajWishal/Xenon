// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "CacheHandler.hpp"

#include <fstream>

std::vector<std::byte> CacheHandler::load(uint64_t hash)
{
	std::vector<std::byte> cacheData;
	auto cacheFile = std::fstream(std::to_string(hash) + ".bin", std::ios::in | std::ios::binary | std::ios::ate);
	if (cacheFile.is_open())
	{
		const auto size = cacheFile.tellg();
		cacheFile.seekg(0);

		cacheData.resize(size);
		cacheFile.read(std::bit_cast<char*>(cacheData.data()), size);

		cacheFile.close();
	}

	return cacheData;
}

void CacheHandler::store(uint64_t hash, const std::vector<std::byte>& bytes)
{
	auto cacheFile = std::fstream(std::to_string(hash) + ".bin", std::ios::out | std::ios::binary);
	if (cacheFile.is_open())
	{
		cacheFile.write(std::bit_cast<const char*>(bytes.data()), bytes.size());
		cacheFile.close();
	}
}
