# Copyright 2022-2023 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	XenonBackend
	VERSION 1.0.0
	DESCRIPTION "The backend library."
)

# Set the sources.
set(
	SOURCES

	"BackendObject.hpp"
	"Instance.hpp"
	"IFactory.hpp"
	"Device.hpp"
	"Buffer.hpp"
	"Camera.hpp"
	"RenderTarget.hpp"
	"Rasterizer.hpp"
	"BufferMemoryView.cpp"
	"BufferMemoryView.hpp"
	"Swapchain.cpp"
	"Swapchain.hpp"
	"Core.hpp"
	"Image.hpp"
	"CommandRecorder.hpp"
	"ShaderSource.cpp"
	"ShaderSource.hpp"
	"Pipeline.hpp"
	"RasterizingPipeline.hpp"
	"Descriptor.hpp"
	"ImageView.hpp"
	"ImageSampler.hpp"
	"ComputePipeline.hpp"
	"CommandSubmitter.hpp"
	"Query.hpp"
	"OcclusionQuery.hpp"
	"AccelerationStructure.hpp"
	"RayTracer.hpp"
	"BottomLevelAccelerationStructure.hpp"
	"TopLevelAccelerationStructure.hpp"
	"RayTracingPipeline.hpp"
	"Shader.cpp"
	"Shader.hpp"
	"ShaderBindingTable.hpp"
	"DescriptorManager.hpp"
)

# Add the source group.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
source_group(ThirdParty FILES "${SPIRV_REFLECT_INCLUDE_DIR}/spirv_reflect.cpp")

# Add the library.
add_library(
	XenonBackend
	STATIC

	"${SPIRV_REFLECT_INCLUDE_DIR}/spirv_reflect.cpp"	# We need this as described by the API.
	${SOURCES}
)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET XenonBackend PROPERTY CXX_STANDARD 20)

# Set the include directories.
target_include_directories(
	XenonBackend 

	PUBLIC ${GLM_INCLUDE_DIR}
	PRIVATE ${SPIRV_REFLECT_INCLUDE_DIR}
)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(XenonBackend PRIVATE "/MP")	
	target_link_libraries(XenonBackend XenonPlatformWindows)
endif ()