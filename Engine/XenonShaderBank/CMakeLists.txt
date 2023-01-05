# Copyright 2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	XenonShaderBank
	VERSION 1.0.0
	DESCRIPTION "This project contains all the shaders used by the engine internally."
)

# Add the shaders.
file(GLOB_RECURSE SHADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/**/*.vert
	${CMAKE_CURRENT_SOURCE_DIR}/**/*.frag
	${CMAKE_CURRENT_SOURCE_DIR}/**/*.comp
	${CMAKE_CURRENT_SOURCE_DIR}/**/*.geom
	${CMAKE_CURRENT_SOURCE_DIR}/**/*.tesc
	${CMAKE_CURRENT_SOURCE_DIR}/**/*.tese
	${CMAKE_CURRENT_SOURCE_DIR}/**/*.mesh
	${CMAKE_CURRENT_SOURCE_DIR}/**/*.task
	${CMAKE_CURRENT_SOURCE_DIR}/**/*.rgen
	${CMAKE_CURRENT_SOURCE_DIR}/**/*.rchit
	${CMAKE_CURRENT_SOURCE_DIR}/**/*.rmiss
)

file(GLOB_RECURSE HLSL_SHADERS
	*.hlsli
	*.hlsl
)

file(GLOB_RECURSE SOURCE_FILES
	*.cpp
	*.hpp
)

# Set the sources.
set(
	SOURCES

	${SHADERS}
	${HLSL_SHADERS}
	${SOURCE_FILES}
)

# Add the source group.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

# Add the custom target.
add_custom_target(
	XenonShaderBank

	SOURCES ${SOURCES}
)

# Include the Shader compiler.
include(${CMAKE_SOURCE_DIR}/Tools/CMake/ShaderCompiler.CMake)

# Compile all the HLSL files.
CompileShaders()