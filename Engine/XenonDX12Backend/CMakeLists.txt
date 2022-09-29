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

	"DX12Instance.cpp"
	"DX12Instance.hpp"
	"DX12Factory.cpp"
	"DX12Factory.hpp"
	"DX12Includes.hpp"
)

# Set the include directories.
target_include_directories(
	XenonDX12Backend 

	PRIVATE ${DIRECT_X_HEADERS_INCLUDE_DIR}
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