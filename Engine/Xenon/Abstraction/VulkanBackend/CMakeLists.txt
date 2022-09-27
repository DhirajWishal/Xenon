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
	
	"ThirdParty/volk.cpp"
	"ThirdParty/vk_mem_alloc.cpp"
)

# Set the include directories.
target_include_directories(
	XenonVulkanBackend 

	PRIVATE ${VULKAN_HEADERS_INCLUDE_DIR}
	PRIVATE ${VOLK_INCLUDE_DIR}
	PRIVATE ${VMA_INCLUDE_DIR}
)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET XenonVulkanBackend PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(XenonVulkanBackend PRIVATE "/MP")	
endif ()
