# Copyright 2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Main build script for Xenon.
# Set the minimum required CMake version.
cmake_minimum_required(VERSION 3.22.2)

# Utility function to generate shaders
#
# @param SHADER_TARGET The target project to add the compiled dependency.
# @param SHADER_SOURCE_DIR The shader file's source directory.
# @param SHADER_BINARY_DIR The compiled output directory.
function(CompileShaders SHADER_TARGET SHADER_SOURCE_DIR SHADER_BINARY_DIR)
	if (DEFINED ENV{VULKAN_SDK})
		file(GLOB SHADERS RELATIVE ${SHADER_SOURCE_DIR}
		  ${SHADER_SOURCE_DIR}/**/*.vert
		  ${SHADER_SOURCE_DIR}/**/*.frag
		  ${SHADER_SOURCE_DIR}/**/*.comp
		  ${SHADER_SOURCE_DIR}/**/*.geom
		  ${SHADER_SOURCE_DIR}/**/*.tesc
		  ${SHADER_SOURCE_DIR}/**/*.tese
		  ${SHADER_SOURCE_DIR}/**/*.mesh
		  ${SHADER_SOURCE_DIR}/**/*.task
		  ${SHADER_SOURCE_DIR}/**/*.rgen
		  ${SHADER_SOURCE_DIR}/**/*.rchit
		  ${SHADER_SOURCE_DIR}/**/*.rmiss
		)

		add_custom_command(
			COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_BINARY_DIR}
			OUTPUT ${SHADER_BINARY_DIR}
			COMMENT "Creating ${SHADER_BINARY_DIR}"
		)

		# Resolve the shader compiler
		if (MSVC)
			set(SHADER_COMPILER $ENV{VULKAN_SDK}\\Bin\\glslangValidator)

		else()
			set(SHADER_COMPILER $ENV{VULKAN_SDK}/bin/glslangValidator)

		endif()

		message("Shader compiler set to " ${SHADER_COMPILER})

		foreach(SOURCE_FILE IN LISTS SHADERS)
			get_filename_component(FILE_NAME ${SOURCE_FILE} NAME)

			message("Compiling " ${SOURCE_FILE} " as " ${FILE_NAME}.spv)
	
			add_custom_command(
				COMMAND ${SHADER_COMPILER} -V ${SHADER_SOURCE_DIR}/${SOURCE_FILE} -o ${SHADER_BINARY_DIR}/${SOURCE_FILE}.spv
				OUTPUT ${SHADER_BINARY_DIR}/${SOURCE_FILE}.spv
				COMMENT "Compiling ${FILE_NAME}"
			)

			list(APPEND SPV_SHADERS ${SHADER_BINARY_DIR}/${SOURCE_FILE}.spv)

		endforeach()

		add_custom_target(${SHADER_TARGET}_ShaderCompiler ALL DEPENDS ${SPV_SHADERS})
		add_dependencies(${SHADER_TARGET} ${SHADER_TARGET}_ShaderCompiler)
	
	else()
		message(WARNING "The VULKAN_SDK environment variable is not set! Automatic shader compilation is disabled.")

	endif()

endfunction()