# Copyright 2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	XenonCommon
	VERSION 1.0.0
	DESCRIPTION "The common library containing all the common data and support structures used throughout the engine."
)

# Add the library.
add_library(
	XenonCommon
	STATIC

	"System.hpp"
	"System.cpp"
	"Logging.hpp"
	"Common.hpp"
)

# Set the include directories.
target_include_directories(
	XenonCommon 

	PUBLIC ${SPDLOG_INCLUDE_DIR}
)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET XenonCommon PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(XenonCommon PRIVATE "/MP")	
endif ()
