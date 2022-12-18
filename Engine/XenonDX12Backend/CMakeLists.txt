# Copyright 2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	XenonDX12Backend
	VERSION 1.0.0
	DESCRIPTION "The DirectX 12 backend library."
)

# Set the sources.
set(
	SOURCES

	"DX12Instance.cpp"
	"DX12Instance.hpp"
	"DX12Factory.cpp"
	"DX12Factory.hpp"
	"DX12Includes.hpp"
	"DX12Device.cpp"
	"DX12Device.hpp"
	"DX12DeviceBoundObject.hpp"
	"DX12Macros.hpp"
	"DX12Buffer.cpp"
	"DX12Buffer.hpp"
	"DX12Rasterizer.cpp"
	"DX12Rasterizer.hpp"
	"DX12CommandRecorder.cpp"
	"DX12CommandRecorder.hpp"
	"DX12Image.cpp"
	"DX12Image.hpp"
	"DX12Swapchain.cpp"
	"DX12Swapchain.hpp"
	"DX12Descriptor.cpp"
	"DX12Descriptor.hpp"
	"DX12ImageView.cpp"
	"DX12ImageView.hpp"
	"DX12ImageSampler.cpp"
	"DX12ImageSampler.hpp"
	"DX12RasterizingPipeline.cpp"
	"DX12RasterizingPipeline.hpp"
	"DX12DescriptorHeapManager.cpp"
	"DX12DescriptorHeapManager.hpp"
	"DX12ComputePipeline.cpp"
	"DX12ComputePipeline.hpp"
	"DX12CommandBufferAllocator.cpp"
	"DX12CommandBufferAllocator.hpp"
	"DX12CommandBuffer.cpp"
	"DX12CommandBuffer.hpp"
)

# Add the source group.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
source_group(ThirdParty FILES "${D3DMA_SOURCE_DIR}/D3D12MemAlloc.cpp")

# Add the library.
add_library(
	XenonDX12Backend
	STATIC

	"${D3DMA_SOURCE_DIR}/D3D12MemAlloc.cpp"	# We need this as described by the API.
	${SOURCES}
)

# Set the include directories.
target_include_directories(
	XenonDX12Backend 

	PRIVATE ${DIRECT_X_HEADERS_INCLUDE_DIR}
	PRIVATE ${D3DMA_INCLUDE_DIR}
	PRIVATE ${SPIRV_CROSS_INCLUDE_DIR}
)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET XenonDX12Backend PROPERTY CXX_STANDARD 20)

# Use the Multi Processor Compilation option.
target_compile_options(XenonDX12Backend PRIVATE "/MP")	

# Set the target links.
target_link_libraries(
	XenonDX12Backend
	PUBLIC XenonBackend 
	
	PRIVATE "d3d12.lib" 
	PRIVATE "dxgi.lib" 
	PRIVATE "d3dcompiler.lib"
	PRIVATE spirv-cross-hlsl
)

# Set compile definitions.
target_compile_definitions(
	XenonDX12Backend 

	PRIVATE "UNICODE" 
	PRIVATE "_UNICODE"
	PRIVATE XENON_DX12_MAX_DESCRIPTOR_COUNT=16
)