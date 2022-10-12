# Copyright 2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	XenonTest
	VERSION 1.0.0
	DESCRIPTION "Xenon testing project."
)

# Add the library.
add_executable(
	XenonTest

	"Main.cpp"
	"Test.hpp"
	"TestContainer.cpp"
	"TestContainer.hpp"
	"InstanceTests/VulkanInstanceTest.cpp"
	"InstanceTests/VulkanInstanceTest.hpp"
	"InstanceTests/DX12InstanceTest.cpp"
	"InstanceTests/DX12InstanceTest.hpp"
)

# Set the include directories.
target_include_directories(
	XenonTest 

	PUBLIC ${CMAKE_SOURCE_DIR}/Engine
)

# Add the target links.
target_link_libraries(XenonTest XenonEngine)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET XenonTest PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(XenonTest PRIVATE "/MP")	
endif ()