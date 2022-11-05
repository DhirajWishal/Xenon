# Copyright 2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	XenonEngine
	VERSION 1.0.0
	DESCRIPTION "The main runtime library."
)

# Add the library.
add_library(
	XenonEngine
	STATIC

	"Instance.cpp"
	"Instance.hpp"
	"RasterizingRenderer.cpp"
	"RasterizingRenderer.hpp"
	"MeshStorage.cpp"
	"MeshStorage.hpp"
	"StaticModel.hpp"
	"VertexSpecification.hpp"
	"MonoCamera.cpp"
	"MonoCamera.hpp"
)

# Set the target links.
target_link_libraries(XenonEngine XenonVulkanBackend)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET XenonEngine PROPERTY CXX_STANDARD 20)

# SEt the include directories.
target_include_directories(
	XenonEngine 

	PRIVATE ${TINYGLTF_INCLUDE_DIR}
	PUBLIC ${GLM_INCLUDE_DIR}
)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(XenonEngine PRIVATE "/MP")	
	target_link_libraries(XenonEngine XenonDX12Backend)
endif ()