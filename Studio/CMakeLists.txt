# Copyright 2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	XenonStudio
	VERSION 1.0.0
	DESCRIPTION "Xenon studio."
)

# Set the sources.
set(
	SOURCES

	"Main.cpp"
	"Studio.cpp"
	"Studio.hpp"
	"CacheHandler.cpp"
	"CacheHandler.hpp"

	"Layers/ImGuiLayer.cpp"
	"Layers/ImGuiLayer.hpp"
)

# Add the source group.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES REGULAR_EXPRESSION ${CMAKE_CURRENT_SOURCE_DIR}/**/*.vert)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES REGULAR_EXPRESSION ${CMAKE_CURRENT_SOURCE_DIR}/**/*.frag)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES REGULAR_EXPRESSION ${CMAKE_CURRENT_SOURCE_DIR}/**/*.comp)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES REGULAR_EXPRESSION ${CMAKE_CURRENT_SOURCE_DIR}/**/*.geom)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES REGULAR_EXPRESSION ${CMAKE_CURRENT_SOURCE_DIR}/**/*.tesc)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES REGULAR_EXPRESSION ${CMAKE_CURRENT_SOURCE_DIR}/**/*.tese)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES REGULAR_EXPRESSION ${CMAKE_CURRENT_SOURCE_DIR}/**/*.mesh)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES REGULAR_EXPRESSION ${CMAKE_CURRENT_SOURCE_DIR}/**/*.task)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES REGULAR_EXPRESSION ${CMAKE_CURRENT_SOURCE_DIR}/**/*.rgen)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES REGULAR_EXPRESSION ${CMAKE_CURRENT_SOURCE_DIR}/**/*.rchit)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES REGULAR_EXPRESSION ${CMAKE_CURRENT_SOURCE_DIR}/**/*.rmiss)

# Add the library.
add_executable(
	XenonStudio

	${SOURCES}
)

# Set the include directories.
target_include_directories(
	XenonStudio 

	PRIVATE ${CMAKE_SOURCE_DIR}/Engine
	PRIVATE ${IMGUI_INCLUDE_DIR}
)

# Add the target links.
target_link_libraries(XenonStudio XenonEngine XenonThirdParty_ImGui)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET XenonStudio PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(XenonStudio PRIVATE "/MP")	
endif ()

# Include the shader compile tool.
include(../Tools/CMake/ShaderCompiler.CMake)

# Setup shader compiler
set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Shaders)
set(SHADER_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/Shaders)

CompileShaders(XenonStudio ${SHADER_SOURCE_DIR} ${SHADER_BINARY_DIR})