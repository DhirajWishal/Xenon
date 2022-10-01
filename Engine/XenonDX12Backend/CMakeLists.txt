# Copyright 2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	XenonDX12Backend
	VERSION 1.0.0
	DESCRIPTION "The DirectX 12 backend library."
)

# Add the library.
add_library(
	XenonDX12Backend
	STATIC

	"${D3DMA_SOURCE_DIR}/D3D12MemAlloc.cpp"	# We need this as described by the API.

	"DX12Instance.cpp"
	"DX12Instance.hpp"
	"DX12Factory.cpp"
	"DX12Factory.hpp"
	"DX12Includes.hpp"
	"DX12Device.cpp"
	"DX12Device.hpp"
	"DX12Macros.hpp"
	"DX12CommandAllocator.cpp"
	"DX12CommandAllocator.hpp"
	"DX12Buffer.cpp"
	"DX12Buffer.hpp"
	"DX12VertexBuffer.cpp"
	"DX12VertexBuffer.hpp"
	"DX12IndexBuffer.cpp" 
	"DX12IndexBuffer.hpp" 
	"DX12StagingBuffer.cpp"
	"DX12StagingBuffer.hpp"
	"DX12StorageBuffer.cpp"
	"DX12StorageBuffer.hpp"
	"DX12UniformBuffer.cpp"
	"DX12UniformBuffer.hpp"
)

# Set the include directories.
target_include_directories(
	XenonDX12Backend 

	PRIVATE ${DIRECT_X_HEADERS_INCLUDE_DIR}
	PRIVATE ${D3DMA_INCLUDE_DIR}
)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET XenonDX12Backend PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(XenonDX12Backend PRIVATE "/MP")	
endif ()

# Set the target links.
target_link_libraries(
	XenonDX12Backend
	PUBLIC XenonBackend 
	
	PRIVATE "d3d12.lib" 
	PRIVATE "dxgi.lib" 
	PRIVATE "d3dcompiler.lib"
)

# Set compile definitions.
target_compile_definitions(XenonDX12Backend PRIVATE "UNICODE" "_UNICODE")