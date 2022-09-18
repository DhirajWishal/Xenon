# Copyright 2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	XenonStudio
	VERSION 1.0.0
	DESCRIPTION "Xenon studio."
)

# Add the library.
add_executable(
	XenonStudio

	"Main.cpp"
)

# Set the include directories.
target_include_directories(
	XenonStudio 

	PUBLIC ${CMAKE_SOURCE_DIR}/Runtime
)

# Add the target links.
target_link_libraries(XenonStudio XenonRuntime)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET XenonStudio PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(XenonStudio PRIVATE "/MP")	
endif ()