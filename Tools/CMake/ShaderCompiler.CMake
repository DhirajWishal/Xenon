# Copyright 2022 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Main build script for Xenon.
# Set the minimum required CMake version.
cmake_minimum_required(VERSION 3.22.2)

if (MSVC)
	set(TOOLS_DIR ${CMAKE_SOURCE_DIR}/Tools/Windows)
endif()

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
				COMMAND ${SHADER_COMPILER} -V ${SHADER_SOURCE_DIR}/${SOURCE_FILE} -o ${SHADER_BINARY_DIR}/${SOURCE_FILE}.spv --target-env vulkan1.2
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

# Utility function to compile HLSL shaders to SPIR-V and DirectX binary.
# This will compile them if the glslangValidator and dxc tools exist. It will place the binary in a file with the following extensions.
# SPIR-V			: <filename>.spv.hpp
# DirectX Binary	: <filename>.dxb.hpp
#
# @param SHADER_SOURCE_DIR The shader file's source directory.
function(CompileHLSL SHADER_SOURCE_DIR)

	# Make a list of all the HLSL files.
	file(GLOB_RECURSE SHADERS RELATIVE ${SHADER_SOURCE_DIR}
		  ${SHADER_SOURCE_DIR}/**/*.hlsl
	)

	# Iterate over the shader files and compile them.
	foreach(SOURCE_FILE IN LISTS SHADERS)
		get_filename_component(FILE_NAME ${SOURCE_FILE} NAME)
		get_filename_component(FILE_NAME_WE ${SOURCE_FILE} NAME_WE)
		get_filename_component(FILE_DIR ${SOURCE_FILE} DIRECTORY)
		string(REPLACE "." "_" FILE_NAME_FORMATTED ${FILE_NAME})
		string(TOUPPER ${FILE_NAME_FORMATTED} FILE_NAME_FORMATTED_UPPER)

		# Set the compile target file.
		set(XENON_COMPILE_TARGET ${FILE_DIR}/${FILE_NAME}.hpp)

		# Write the initial contents of the HLSL source file which will include all the shaders.
		file(
			WRITE 
			${XENON_COMPILE_TARGET} 

			"// Copyright 2022 Dhiraj Wishal\n"
			"// SPDX-License-Identifier: Apache-2.0\n"
			"\n"
			"#pragma once\n"
			"\n"
			"#include \"${CMAKE_SOURCE_DIR}/Engine/XenonBackend/Shader.hpp\"\n"
			"\n"
			"#include <cstdint>\n"
			"\n"
		)

		if(MSVC)
			# Resolve the target profile.
			if (${SOURCE_FILE} MATCHES "vert")
				set(TARGET_PROFILE vs_6_6)

			elseif (${SOURCE_FILE} MATCHES "frag")
				set(TARGET_PROFILE ps_6_6)

			elseif (${SOURCE_FILE} MATCHES "comp")
				set(TARGET_PROFILE cs_6_6)

			elseif (${SOURCE_FILE} MATCHES "geom")
				set(TARGET_PROFILE gs_6_6)

			elseif (${SOURCE_FILE} MATCHES "tesc")
				set(TARGET_PROFILE hs_6_6)

			elseif (${SOURCE_FILE} MATCHES "tese")
				set(TARGET_PROFILE ds_6_6)

			else()
				set(TARGET_PROFILE lib_6_6)

			endif()

			# Compile and store the HLSL binary in the header file.
			execute_process(
				COMMAND ${TOOLS_DIR}/dxc -Fh ${SOURCE_FILE}.dxb.hpp -I ${CMAKE_SOURCE_DIR}/Engine/ShaderBank -HV 2021 -Vn g_${FILE_NAME_FORMATTED_UPPER}_HLSL -T ${TARGET_PROFILE} -E main ${SOURCE_FILE}
				WORKING_DIRECTORY ${SHADER_SOURCE_DIR}
				COMMAND_ECHO STDOUT
				ERROR_VARIABLE SHADER_COMPILER_STATUS
			)

			# Show the status.
			message(STATUS ${SHADER_COMPILER_STATUS})

			# Include the header file.
			file(
				APPEND 
				${XENON_COMPILE_TARGET} 

				"#include \"${FILE_NAME}.dxb.hpp\"\n"
			)

		endif()

		# Compile and store the SPIR-V binary in the header file.
		execute_process(
			COMMAND ${TOOLS_DIR}/glslangValidator -V ${SOURCE_FILE} -o ${SOURCE_FILE}.spv.hpp --variable-name g_${FILE_NAME_FORMATTED_UPPER}_SPIRV --target-env vulkan1.3 -I${CMAKE_SOURCE_DIR}/Engine/ShaderBank --entry-point main
			WORKING_DIRECTORY ${SHADER_SOURCE_DIR}
			COMMAND_ECHO STDOUT
			ERROR_VARIABLE SHADER_COMPILER_STATUS
		)

		# Show the status.
		message(STATUS ${SHADER_COMPILER_STATUS})

		# Include the header file.
		file(
			APPEND 
			${XENON_COMPILE_TARGET} 
			
			"#include \"${FILE_NAME}.spv.hpp\"\n"
			"\n"
		)

		if(MSVC)
			file(
				APPEND 
				${XENON_COMPILE_TARGET} 
				
				"namespace Xenon { namespace Generated { [[nodisacrd]] Xenon::Backend::Shader CreateShader${FILE_NAME_WE}() { return Xenon::Backend::Shader::Create(g_${FILE_NAME_FORMATTED_UPPER}_SPIRV, g_${FILE_NAME_FORMATTED_UPPER}_HLSL); } } }\n"
			)

		else()
			file(
				APPEND 
				${XENON_COMPILE_TARGET} 
				
				"namespace Xenon { namespace Generated { [[nodisacrd]] Xenon::Backend::Shader CreateShader${FILE_NAME_WE}() { return Xenon::Backend::Shader::Create(g_${FILE_NAME_FORMATTED_UPPER}_SPIRV); } } }\n"
			)

		endif()

	endforeach()

endfunction()