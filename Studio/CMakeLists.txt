# Copyright 2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	XenonStudio
	VERSION 1.0.0
	DESCRIPTION "Xenon studio."
)

# Set the sources.
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

set(
	SOURCES

	"Main.cpp"
	"Studio.cpp"
	"Studio.hpp"
	"CacheHandler.cpp"
	"CacheHandler.hpp"
	"UIComponent.hpp"

	"Layers/ImGuiLayer.cpp"
	"Layers/ImGuiLayer.hpp"

	"Materials/ImGuiMaterial.cpp"
	"Materials/ImGuiMaterial.hpp"

	"UIComponents/PerformanceMetrics.cpp"
	"UIComponents/PerformanceMetrics.hpp"
	"UIComponents/LayerView.cpp"
	"UIComponents/LayerView.hpp"

	${SHADERS}
)

# Add the source group.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

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