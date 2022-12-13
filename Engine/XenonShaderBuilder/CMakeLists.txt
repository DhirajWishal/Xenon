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

	"ShaderBuilder.cpp"
	"ShaderBuilder.hpp"
	"TypeTraits.hpp"
	"Input.hpp"
	"AssemblyStorage.cpp"
	"AssemblyStorage.hpp"
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
)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(XenonShaderBuilder PRIVATE "/MP")	
	target_link_libraries(XenonShaderBuilder XenonCore)
endif ()