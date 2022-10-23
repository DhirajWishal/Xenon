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

	"ThirdParty/volk.cpp"
	"ThirdParty/vk_mem_alloc.cpp"
	
	"VulkanInstance.cpp"
	"VulkanInstance.hpp"
	"VulkanFactory.cpp"
	"VulkanFactory.hpp"
	"VulkanMacros.hpp"
	"VulkanDevice.cpp"
	"VulkanDevice.hpp"
	"VulkanQueue.cpp"
	"VulkanQueue.hpp"
	"VulkanCommandBuffer.cpp"
	"VulkanCommandBuffer.hpp"
	"VulkanBuffer.cpp"
	"VulkanBuffer.hpp"
	"VulkanRasterizer.cpp"
	"VulkanRasterizer.hpp"
	"VulkanCommandRecorder.cpp"
	"VulkanCommandRecorder.hpp"
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

# Set the target links.
target_link_libraries(XenonVulkanBackend XenonBackend)