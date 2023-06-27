# Copyright 2022-2023 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	XenonPlatformLinux
	VERSION 1.0.0
	DESCRIPTION "The platform Linux library."
)

# Set the sources.
set(
	SOURCES

	"LinuxFactory.cpp"
	"LinuxFactory.hpp"
	"LinuxWindow.cpp"
	"LinuxWindow.hpp"
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
target_link_libraries(XenonPlatformLinux XenonPlatform SDL3-shared)

# Copy the SDL shared library to the output directory.
add_custom_command(
	TARGET XenonPlatformLinux 
	POST_BUILD

	COMMENT "Copying SDL shared library to the output directory."
	COMMAND ${CMAKE_COMMAND} -E copy
		$<TARGET_FILE:SDL3-shared>
		$<TARGET_FILE_DIR:XenonPlatformLinux>/$<TARGET_FILE_NAME:SDL3-shared>
)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET XenonPlatformLinux PROPERTY CXX_STANDARD 20)
