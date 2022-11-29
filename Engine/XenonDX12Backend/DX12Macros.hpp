// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../XenonCore/Logging.hpp"

#include <system_error>

#define XENON_DX12_ASSERT(exp, ...)				XENON_ASSERT(SUCCEEDED(exp), "Direct X 12: " __VA_ARGS__)
#define XENON_DX12_BLOB_TO_STRING(blob)			blob ? std::string(std::bit_cast<const char*>(blob->GetBufferPointer()), blob->GetBufferSize()) : std::string()
#define XENON_DX12_LOG_HRESULT(result)			XENON_LOG_INFORMATION("HRESULT: {}", std::system_category().message(result))
#define XENON_DX12_LOG_BLOB_HASH(msg, blob)		XENON_LOG_INFORMATION(msg ": {}", ::Xenon::GenerateHash(::Xenon::ToBytes(blob->GetBufferPointer()), blob->GetBufferSize()))

#define XENON_DX12_UNIQUE_LINE_VARIABLE(...)	_variable_##__VA_ARGS__
#define XENON_DX12_ASSERT_BLOB(blob, ...)													\
	const auto XENON_DX12_UNIQUE_LINE_VARIABLE(__LINE__) = XENON_DX12_BLOB_TO_STRING(blob);	\
	if(!XENON_DX12_UNIQUE_LINE_VARIABLE(__LINE__).empty())									\
		XENON_LOG_FATAL("D3D12 blob: {}", XENON_DX12_UNIQUE_LINE_VARIABLE(__LINE__))
