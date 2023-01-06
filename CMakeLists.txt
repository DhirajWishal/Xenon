# Copyright 2022-2023 Dhiraj Wishal
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

# Set the caches.
set(XENON_LOG_LEVEL 5 CACHE INTERNAL "This defines what to log. Checkout the wiki page for more information.")

# Add the third party libraries.
set(SPDLOG_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/spdlog/include)
set(VULKAN_HEADERS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/Vulkan-Headers/include)
set(VOLK_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/volk)
set(VMA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/VulkanMemoryAllocator/include)
set(DIRECT_X_HEADERS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/DirectX-Headers/include)
set(D3DMA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/D3D12MemoryAllocator/include)
set(D3DMA_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/D3D12MemoryAllocator/src)
set(TINYGLTF_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/tinygltf)
set(GLM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glm)
set(SPIRV_REFLECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/SPIRV-Reflect)
set(XXHASH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/xxHash)
set(OPTICK_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/optick/src)
set(IMGUI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui)
set(IMNODES_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imnodes)
set(FMT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/fmt/include)

# Add the Optick library as a third party static link.
add_library(
	XenonThirdParty_Optick
	STATIC

	"${OPTICK_INCLUDE_DIR}/optick.config.h"
	"${OPTICK_INCLUDE_DIR}/optick.h"
	"${OPTICK_INCLUDE_DIR}/optick_capi.cpp"
	"${OPTICK_INCLUDE_DIR}/optick_capi.h"
	"${OPTICK_INCLUDE_DIR}/optick_common.h"
	"${OPTICK_INCLUDE_DIR}/optick_core.cpp"
	"${OPTICK_INCLUDE_DIR}/optick_core.freebsd.h"
	"${OPTICK_INCLUDE_DIR}/optick_core.h"
	"${OPTICK_INCLUDE_DIR}/optick_core.linux.h"
	"${OPTICK_INCLUDE_DIR}/optick_core.macos.h"
	"${OPTICK_INCLUDE_DIR}/optick_core.platform.h"
	"${OPTICK_INCLUDE_DIR}/optick_core.win.h"
	"${OPTICK_INCLUDE_DIR}/optick_gpu.cpp"
	"${OPTICK_INCLUDE_DIR}/optick_gpu.d3d12.cpp"
	"${OPTICK_INCLUDE_DIR}/optick_gpu.h"
	"${OPTICK_INCLUDE_DIR}/optick_gpu.vulkan.cpp"
	"${OPTICK_INCLUDE_DIR}/optick_memory.h"
	"${OPTICK_INCLUDE_DIR}/optick_message.cpp"
	"${OPTICK_INCLUDE_DIR}/optick_message.h"
	"${OPTICK_INCLUDE_DIR}/optick_miniz.cpp"
	"${OPTICK_INCLUDE_DIR}/optick_miniz.h"
	"${OPTICK_INCLUDE_DIR}/optick_serialization.cpp"
	"${OPTICK_INCLUDE_DIR}/optick_serialization.h"
	"${OPTICK_INCLUDE_DIR}/optick_server.cpp"
	"${OPTICK_INCLUDE_DIR}/optick_server.h"
)

# Set the include directories.
target_include_directories(
	XenonThirdParty_Optick 

	PRIVATE ${VULKAN_HEADERS_INCLUDE_DIR}
)

# Add the ImGui library as a third party static link.
add_library(
	XenonThirdParty_ImGui
	STATIC 

	"${IMGUI_INCLUDE_DIR}/imconfig.h"
	"${IMGUI_INCLUDE_DIR}/imgui.cpp"
	"${IMGUI_INCLUDE_DIR}/imgui.h"
	"${IMGUI_INCLUDE_DIR}/imgui_demo.cpp"
	"${IMGUI_INCLUDE_DIR}/imgui_draw.cpp"
	"${IMGUI_INCLUDE_DIR}/imgui_internal.h"
	"${IMGUI_INCLUDE_DIR}/imgui_tables.cpp"
	"${IMGUI_INCLUDE_DIR}/imgui_widgets.cpp"
	"${IMGUI_INCLUDE_DIR}/imstb_rectpack.h"
	"${IMGUI_INCLUDE_DIR}/imstb_textedit.h"
	"${IMGUI_INCLUDE_DIR}/imstb_truetype.h"
)

# Add imnodes library as a third party static link.
add_library(
	XenonThirdParty_imnodes
	STATIC 

	"${IMNODES_INCLUDE_DIR}/imnodes.cpp"
	"${IMNODES_INCLUDE_DIR}/imnodes.h"
)

# Set the include directories.
target_include_directories(
	XenonThirdParty_imnodes 

	PRIVATE ${IMGUI_INCLUDE_DIR}
)

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

	XENON_GLTF_ASSET_DIR="${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glTF-Sample-Models/"
	XENON_LOG_LEVEL=${XENON_LOG_LEVEL}

	FMT_HEADER_ONLY

	# Conditionally enable ray tracing. 
	# This is required since not all devices support ray tracing for all backends (for me Vulkan ray tracing does not work).
	# Comment this out to disable ray tracing from the studio.
	# XENON_DEV_ENABLE_RAY_TRACING
)

# Include the main subdirectories.
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Engine/Xenon)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Engine/XenonBackend)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Engine/XenonCore)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Engine/XenonVulkanBackend)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Engine/XenonDX12Backend)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Engine/XenonPlatform)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Engine/XenonPlatformWindows)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Engine/XenonEvents)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Engine/XenonShaderBank)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Studio)

# Set the output directories.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set the startup project for Visual Studio and set multi processor compilation for other projects that we build.
if (MSVC) 
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT XenonStudio)
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)

	# Add a custom target for Visual Studio to build the solution files again.
	add_custom_target(
		XenonConfigureCMake
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMAND ./Scripts/Bootstrap.bat
	)
	 
	# Add the custom target to a folder so it won't clutter up the solution explorer.
	set_target_properties(XenonConfigureCMake PROPERTIES FOLDER "VisualStudio")

	# Add the third party targets to a third party folder.
	set_target_properties(
		XenonThirdParty_Optick
		XenonThirdParty_ImGui
		XenonThirdParty_imnodes
			
		PROPERTIES FOLDER "ThirdParty"
	)

endif ()