# Copyright 2022-2023 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	XenonPlatform
	VERSION 1.0.0
	DESCRIPTION "The platform library."
)

# Set the sources.
set(
	SOURCES

	"IFactory.cpp"
	"IFactory.hpp"
	"Window.hpp"
)

# Add the source group.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

# Add the library.
add_library(
	XenonPlatform
	STATIC

	${SOURCES}
)

# Set the target links.
target_link_libraries(XenonPlatform XenonCore XenonEvents)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET XenonPlatform PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(XenonPlatform PRIVATE "/MP")	
endif ()