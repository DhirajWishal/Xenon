# Copyright 2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Set the basic project information.
project(
	XenonShaderBank
	VERSION 1.0.0
	DESCRIPTION "This project contains all the shaders used by the engine internally."
)

# Include the Shader compiler.
include(${CMAKE_SOURCE_DIR}/Tools/CMake/ShaderCompiler.CMake)

# Compile all the HLSL files.
CompileHLSL(${CMAKE_CURRENT_SOURCE_DIR})

# Add the shaders.
file(GLOB_RECURSE SHADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/**/*.vert
	${CMAKE_CURRENT_SOURCE_DIR}/**/*.frag
	${CMAKE_CURRENT_SOURCE_DIR}/**/*.comp
	${CMAKE_CURRENT_SOURCE_DIR}/**/*.geom
	${CMAKE_CURRENT_SOURCE_DIR}/**/*.tesc
	${CMAKE_CURRENT_SOURCE_DIR}/**/*.tese
	${CMAKE_CURRENT_SOURCE_DIR}/**/*.mesh
	${CMAKE_CURRENT_SOURCE_DIR}/**/*.task
	${CMAKE_CURRENT_SOURCE_DIR}/**/*.rgen
	${CMAKE_CURRENT_SOURCE_DIR}/**/*.rchit
	${CMAKE_CURRENT_SOURCE_DIR}/**/*.rmiss
)

file(GLOB_RECURSE HLSL_SHADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/**/*.hlsl
)

file(GLOB_RECURSE SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/**/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.hpp

)

# Set the sources.
set(
	SOURCES
	${SHADERS}
	${HLSL_SHADERS}
	${SOURCE_FILES}
)

set_property(SOURCE ${HLSL_SHADERS} PROPERTY VS_SETTINGS "ExcludedFromBuild=true")

# Add the source group.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

# Add the custom target.
if (DEFINED ENV{VULKAN_SDK})
	# Resolve the shader compiler
	if (MSVC)
		set(SHADER_COMPILER $ENV{VULKAN_SDK}\\Bin\\glslangValidator)

	else()
		set(SHADER_COMPILER $ENV{VULKAN_SDK}/bin/glslangValidator)

	endif()

	message("Shader compiler set to " ${SHADER_COMPILER})

	# Iterate over the source shaders and compile them.
	foreach(SOURCE_FILE IN ITEMS ${SOURCES})
		if(${SOURCE_FILE} MATCHES "hlsl|hpp")
			continue()
		endif()

		get_filename_component(FILE_NAME ${SOURCE_FILE} NAME)
		get_filename_component(FILE_DIR ${CMAKE_CURRENT_BINARY_DIR}/${SOURCE_FILE}.spv DIRECTORY)

		message("Creating the output directory " ${FILE_DIR})
		file(MAKE_DIRECTORY ${FILE_DIR})

		message("Compiling " ${SOURCE_FILE} " as " ${FILE_NAME}.spv " to " ${CMAKE_CURRENT_BINARY_DIR}/${SOURCE_FILE}.spv)

		add_custom_command(
			COMMAND ${SHADER_COMPILER} -V ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE} -o ${CMAKE_CURRENT_BINARY_DIR}/${SOURCE_FILE}.spv --target-env vulkan1.2
			OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${SOURCE_FILE}.spv
			COMMENT "Compiling ${FILE_NAME}"
		)

		list(APPEND SPV_SHADERS ${CMAKE_CURRENT_BINARY_DIR}/${SOURCE_FILE}.spv)

	endforeach()

	add_custom_target(
		XenonShaderBank 
		ALL DEPENDS ${SPV_SHADERS}
		SOURCES ${SOURCES}
	)

else()
	add_custom_target(
		XenonShaderBank
		SOURCES ${SOURCES}
	)

	message(WARNING "The VULKAN_SDK environment variable is not set! Automatic shader compilation is disabled.")

endif()

# Add the shader output directory to a variable.
set(XENON_SHADER_BANK_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/" PARENT_SCOPE)