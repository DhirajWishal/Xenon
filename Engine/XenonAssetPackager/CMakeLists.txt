# Copyright 2022-2023 Nexonous
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	XenonAssetPackager
	VERSION 1.0.0
	DESCRIPTION "The asset packager application."
)

# Set the sources.
set(
	SOURCES

	"Main.cpp"
	"Packager.cpp"
	"Packager.hpp"
)

# Add the source group.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

# Add the executable.
add_executable(
	XenonAssetPackager

	${SOURCES}
)

# Set the target links.
target_link_libraries(XenonAssetPackager XenonCore)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET XenonAssetPackager PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(XenonAssetPackager PRIVATE "/MP")	
endif ()