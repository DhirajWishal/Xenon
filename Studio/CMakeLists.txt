# Copyright 2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	XenonStudio
	VERSION 1.0.0
	DESCRIPTION "Xenon studio."
)

# Set the sources.
set(
	SOURCES

	"Main.cpp"
	"Studio.cpp"
	"Studio.hpp"
)

# Add the source group.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

# Add the library.
add_executable(
	XenonStudio

	${SOURCES}
)

# Set the include directories.
target_include_directories(
	XenonStudio 

	PUBLIC ${CMAKE_SOURCE_DIR}/Engine
)

# Add the target links.
target_link_libraries(XenonStudio XenonEngine)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET XenonStudio PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(XenonStudio PRIVATE "/MP")	
endif ()