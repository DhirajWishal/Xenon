# Copyright 2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	XenonEngine
	VERSION 1.0.0
	DESCRIPTION "The main runtime library."
)

# Set the sources.
set(
	SOURCES

	"Instance.cpp"
	"Instance.hpp"
	"Renderer.cpp"
	"Renderer.hpp"
	"MeshStorage.cpp"
	"MeshStorage.hpp"
	"StaticModel.hpp"
	"MonoCamera.cpp"
	"MonoCamera.hpp"
	"Layer.cpp"
	"Layer.hpp"
	"RasterizingLayer.cpp"
	"RasterizingLayer.hpp"
	"FrameTimer.cpp"
	"FrameTimer.hpp"
	"MaterialBlob.hpp"
	"MaterialDatabase.hpp"
	"AssetManager.cpp"
	"AssetManager.hpp"
	"Package.cpp"
	"Package.hpp"
	"RayTracingLayer.cpp"
	"RayTracingLayer.hpp"

	"Layers/ClearScreenLayer.cpp"
	"Layers/ClearScreenLayer.hpp"
	"Layers/DefaultRasterizingLayer.cpp"
	"Layers/DefaultRasterizingLayer.hpp"
	"Layers/DefaultRayTracingLayer.cpp"
	"Layers/DefaultRayTracingLayer.hpp"

	"Materials/DefaultMaterial.cpp"
	"Materials/DefaultMaterial.hpp"
	"Materials/PBRMetallicRoughnessMaterial.cpp"
	"Materials/PBRMetallicRoughnessMaterial.hpp"
)

# Add the source group.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

# Add the library.
add_library(
	XenonEngine
	STATIC

	${SOURCES}
)

# Set the target links.
target_link_libraries(XenonEngine XenonVulkanBackend)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET XenonEngine PROPERTY CXX_STANDARD 20)

# Add the engine dependencies.
add_dependencies(XenonEngine XenonShaderBank)

# Set the include directories.
target_include_directories(
	XenonEngine 

	PRIVATE ${TINYGLTF_INCLUDE_DIR}
	PRIVATE ${STB_INCLUDE_DIR}
)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(XenonEngine PRIVATE "/MP")	
	target_link_libraries(XenonEngine XenonDX12Backend)
endif ()