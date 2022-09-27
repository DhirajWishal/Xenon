# Copyright 2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	XenonVulkanBackend
	VERSION 1.0.0
	DESCRIPTION "The Vulkan backend library."
)

# Add the library.
add_library(
	XenonVulkanBackend
	STATIC

	"VulkanInstance.cpp"
	"VulkanInstance.hpp"
)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET XenonVulkanBackend PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(XenonVulkanBackend PRIVATE "/MP")	
endif ()
