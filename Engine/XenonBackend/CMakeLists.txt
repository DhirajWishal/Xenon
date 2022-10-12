# Copyright 2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	XenonBackend
	VERSION 1.0.0
	DESCRIPTION "The backend library."
)

# Add the library.
add_library(
	XenonBackend
	STATIC

	"BackendObject.hpp"
	"Instance.cpp"
	"Instance.hpp"
	"IFactory.hpp"
	"Device.hpp"
	"Buffer.hpp"
	"HostAccessibleBuffer.hpp"
	"StagingBuffer.hpp"
	"VertexBuffer.hpp"
	"IndexBuffer.hpp"
	"UniformBuffer.hpp"
	"Camera.hpp"
	"RenderTarget.hpp"
)

# Add the target links.
target_link_libraries(XenonBackend XenonCore)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET XenonBackend PROPERTY CXX_STANDARD 20)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(XenonBackend PRIVATE "/MP")	
endif ()