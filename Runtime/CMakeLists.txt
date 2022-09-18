# Copyright 2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	XenonRuntime
	VERSION 1.0.0
	DESCRIPTION "The main runtime library."
)

# Add the library.
add_library(
	XenonRuntime
	STATIC

	"Xenon.hpp"

	"Source/Xenon.cpp"
)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET XenonRuntime PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(XenonRuntime PRIVATE "/MP")	
endif ()

# Include the subdirectories.
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Common)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/GameFramework)