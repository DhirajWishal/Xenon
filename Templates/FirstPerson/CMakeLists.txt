# Copyright 2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	FirstPerson
	VERSION 1.0.0
	DESCRIPTION "First person template."
)

# Add the library.
add_executable(
	FirstPerson

	"FirstPerson.hpp"
	"FirstPerson.cpp"
	"Player.hpp"
	"Player.cpp"
)

# Set the include directories.
target_include_directories(
	FirstPerson 

	PUBLIC ${CMAKE_SOURCE_DIR}/Runtime
)

# Add the target links.
target_link_libraries(FirstPerson XenonRuntime)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET FirstPerson PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(FirstPerson PRIVATE "/MP")	
endif ()