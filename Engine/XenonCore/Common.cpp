// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Common.hpp"

#define XXH_INLINE_ALL
#include <xxhash.h>

namespace Xenon
{
	uint64_t GenerateHash(const std::byte* pBytes, uint64_t size, uint64_t seed /*= 0*/) noexcept
	{
		return XXH64(pBytes, size, seed);
	}
}