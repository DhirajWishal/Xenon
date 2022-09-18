# Copyright 2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	XenonLauncher
	VERSION 1.0.0
	DESCRIPTION "Xenon engine launcher."
)

# Add the library.
add_executable(
	XenonLauncher

	"Main.cpp"
)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET XenonLauncher PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(XenonLauncher PRIVATE "/MP")	
endif ()