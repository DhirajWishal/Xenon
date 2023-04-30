# Copyright 2022-2023 Nexonous
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	XenonPlatformWindows
	VERSION 1.0.0
	DESCRIPTION "The platform windows library."
)

# Set the sources.
set(
	SOURCES

	"WindowsFactory.cpp"
	"WindowsFactory.hpp"
	"WindowsWindow.cpp"
	"WindowsWindow.hpp"
)

# Add the source group.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

# Add the library.
add_library(
	XenonPlatformWindows
	STATIC

	${SOURCES}
)

# Set the target links.
target_link_libraries(XenonPlatformWindows XenonPlatform)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET XenonPlatformWindows PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(XenonPlatformWindows PRIVATE "/MP")	
endif ()