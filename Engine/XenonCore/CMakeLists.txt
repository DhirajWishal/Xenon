# Copyright 2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	XenonCore
	VERSION 1.0.0
	DESCRIPTION "The core library."
)

# Add the library.
add_library(
	XenonCore
	STATIC

	"Common.hpp"
	"JobSystem.cpp"
	"JobSystem.hpp"
	"Logging.hpp"
	"SparseArray.hpp"
	"Logging.cpp"
	"XObject.cpp"
	"XObject.hpp"
	"BitSet.hpp"
)

# Set the include directories.
target_include_directories(
	XenonCore 

	PUBLIC ${SPDLOG_INCLUDE_DIR}
)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET XenonCore PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(XenonCore PRIVATE "/MP")	
endif ()