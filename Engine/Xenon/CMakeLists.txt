# Copyright 2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	XenonEngine
	VERSION 1.0.0
	DESCRIPTION "The main runtime library."
)

# Add the library.
add_library(
	XenonEngine
	STATIC

	"Common.hpp"
	"JobSystem.cpp"
	"JobSystem.hpp"
	"Logging.hpp"
	"SparseArray.hpp"
)

# Set the target links.
target_link_libraries(XenonEngine XenonAbstraction)

# Set the include directories.
target_include_directories(
	XenonEngine 

	PUBLIC ${SPDLOG_INCLUDE_DIR}
)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET XenonEngine PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(XenonEngine PRIVATE "/MP")	
endif ()

# Include the subdirectories.
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Abstraction)