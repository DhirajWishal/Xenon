# Copyright 2022-2023 Nexonous
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	XenonEvents
	VERSION 1.0.0
	DESCRIPTION "The core events library."
)

# Set the sources.
set(
	SOURCES

	"Dummy.cpp"
	"Mouse.hpp"
	"Keyboard.hpp"
)

# Add the source group.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

# Add the library.
add_library(
	XenonEvents
	STATIC

	${SOURCES}
)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET XenonEvents PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(XenonEvents PRIVATE "/MP")	
endif ()