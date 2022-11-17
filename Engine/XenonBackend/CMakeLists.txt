# Copyright 2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	XenonBackend
	VERSION 1.0.0
	DESCRIPTION "The backend library."
)

# Set the sources.
set(
	SOURCES

	"BackendObject.hpp"
	"Instance.hpp"
	"IFactory.hpp"
	"Device.hpp"
	"Buffer.hpp"
	"Camera.hpp"
	"RenderTarget.hpp"
	"Rasterizer.hpp"
	"BufferMemoryView.cpp"
	"BufferMemoryView.hpp"
	"Swapchain.cpp"
	"Swapchain.hpp"
	"Core.hpp"
	"Image.hpp"
	"CommandRecorder.hpp"
	"ShaderSource.cpp"
	"ShaderSource.hpp"
	"Pipeline.hpp"
	"RasterizingPipeline.hpp"
)

# Add the source group.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

# Add the library.
add_library(
	XenonBackend
	STATIC

	${SOURCES}
)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET XenonBackend PROPERTY CXX_STANDARD 20)

# Set the include directories.
target_include_directories(
	XenonBackend 

	PUBLIC ${GLM_INCLUDE_DIR}
)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(XenonBackend PRIVATE "/MP")	
	target_link_libraries(XenonBackend XenonPlatformWindows)
endif ()