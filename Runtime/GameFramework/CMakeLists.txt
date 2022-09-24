# Copyright 2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	XenonGameFramework
	VERSION 1.0.0
	DESCRIPTION "The game framework containing all the game components."
)

# Add the library.
add_library(
	XenonGameFramework
	STATIC

	"Entity.hpp"
	"Entity.cpp"
	"UniversalEntity.hpp"
)

# Add the target links.
target_link_libraries(XenonGameFramework XenonCommon)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET XenonGameFramework PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(XenonGameFramework PRIVATE "/MP")	
endif ()
