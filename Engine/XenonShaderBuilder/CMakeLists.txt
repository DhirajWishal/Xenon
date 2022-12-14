# Copyright 2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	XenonShaderBuilder
	VERSION 1.0.0
	DESCRIPTION "This library can be used to build almost any type of shaders in SPIR-V."
)

# Set the sources.
set(
	SOURCES

	"Builder.cpp"
	"Builder.hpp"
	"TypeTraits.hpp"
	"Input.hpp"
	"AssemblyStorage.cpp"
	"AssemblyStorage.hpp"
	"DataType.hpp"
	"Output.hpp"
	"Function.hpp"
	"Variable.hpp"
	"VertexShader.cpp"
	"VertexShader.hpp"
	"Uniform.hpp"
	"Buffer.hpp"
	"BuiltIn.hpp"
)

# Add the source group.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

# Add the library.
add_library(
	XenonShaderBuilder
	STATIC

	${SOURCES}
)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET XenonShaderBuilder PROPERTY CXX_STANDARD 20)

# Set the include directories.
target_include_directories(
	XenonShaderBuilder 

	PUBLIC ${GLM_INCLUDE_DIR}
	PUBLIC ${FMT_INCLUDE_DIR}
	PRIVATE ${SPIRV_TOOLS_INCLUDE_DIR}
)

# Add the target links.
target_link_libraries(
	XenonShaderBuilder 

	PUBLIC XenonBackend 
	PUBLIC SPIRV-Tools-static
	PUBLIC SPIRV-Tools-opt
)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(XenonShaderBuilder PRIVATE "/MP")	
endif ()