# Copyright 2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	XenonAbstraction
	VERSION 1.0.0
	DESCRIPTION "The abstraction library."
)

# Add the library.
add_library(
	XenonAbstraction
	STATIC

	"Instance.cpp"
	"Instance.hpp"
)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET XenonAbstraction PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(XenonAbstraction PRIVATE "/MP")	
endif ()

# Include the subdirectories.
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/VulkanBackend)

# Set the target links.
target_link_libraries(XenonAbstraction XenonVulkanBackend)