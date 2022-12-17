# Copyright 2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	XenonPlatformLinux
	VERSION 1.0.0
	DESCRIPTION "The platform linux library."
)

# Set the sources.
set(
	SOURCES

	"LinuxFactory.cpp"
	"LinuxFactory.hpp"
)

# Add the source group.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

# Add the library.
add_library(
	XenonPlatformLinux
	STATIC

	${SOURCES}
)

# Set the target links.
target_link_libraries(XenonPlatformLinux XenonPlatform)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET XenonPlatformLinux PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(XenonPlatformLinux PRIVATE "/MP")	
endif ()