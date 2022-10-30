# Copyright 2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Main build script for Xenon.
# Set the minimum required CMake version.
cmake_minimum_required(VERSION 3.22.2)

# Set the basic project description.
project(
	Xenon
	VERSION 1.0.0 
	DESCRIPTION "Cross-platform graphics engine."
)

# Lets tell CMake to add the default ALL_BUILD, ZERO_CHECK and INSTALL to a group.
# This way we can make things much more simpler for Visual Studio.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(PREDEFINED_TARGETS_FOLDER "PredefinedTargets")

# Add the third party includes.
set(SPDLOG_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/spdlog/include)
set(VULKAN_HEADERS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/Vulkan-Headers/include)
set(VOLK_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/volk)
set(VMA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/VulkanMemoryAllocator/include)
set(DIRECT_X_HEADERS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/DirectX-Headers/include)
set(D3DMA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/D3D12MemoryAllocator/include)
set(D3DMA_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/D3D12MemoryAllocator/src)
set(TINYGLTF_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/tinygltf)

# Set global compile definitions.
add_compile_definitions(
	$<$<CONFIG:Debug>:XENON_DEBUG>
	$<$<CONFIG:Debug>:USE_OPTICK>
	$<$<CONFIG:Release>:XENON_RELEASE>

	$<$<CONFIG:RelWithDebInfo>:XENON_DEBUG>
	$<$<CONFIG:RelWithDebInfo>:USE_OPTICK>
	$<$<CONFIG:MinSizeRel>:XENON_RELEASE>
	
	$<$<PLATFORM_ID:Windows>:XENON_PLATFORM_WINDOWS>
	$<$<PLATFORM_ID:Linux>:XENON_PLATFORM_LINUX>
	$<$<PLATFORM_ID:Darwin>:XENON_PLATFORM_MAC>
)

# Include the main subdirectories.
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Engine/Xenon)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Engine/XenonBackend)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Engine/XenonCore)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Engine/XenonVulkanBackend)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Engine/XenonDX12Backend)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Studio)

# Set the output directories.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set the startup project for Visual Studio and set multi processor compilation for other projects that we build.
if (MSVC) 
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT XenonStudio)
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif ()