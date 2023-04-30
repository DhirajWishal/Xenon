# Copyright 2022-2023 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Main build script for Xenon.
# Set the minimum required CMake version.
cmake_minimum_required(VERSION 3.22.2)

if (MSVC)
	set(TOOLS_DIR ${CMAKE_SOURCE_DIR}/Tools/Windows)
endif()

# Utility function to compile HLSL shaders to SPIR-V and DirectX binary which are found in subdirectories.
# This will compile them if the glslangValidator and dxc tools exist. It will place the binary in a file with the following extensions.
# SPIR-V			: <filename>.spirv.hpp
# DirectX Binary	: <filename>.dxil.hpp
function(CompileShaders)
	# Make a list of all the HLSL files.
	file(GLOB_RECURSE SHADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
		  *.hlsl
	)

	# Set the shader model for VS.
	set_source_files_properties(${SHADERS} PROPERTIES ShaderModel "6_6")

	# Iterate over the shader files and compile them.
	foreach(SOURCE_FILE IN LISTS SHADERS)
		get_filename_component(FILE_NAME ${SOURCE_FILE} NAME)
		get_filename_component(FILE_NAME_WE ${SOURCE_FILE} NAME_WE)
		get_filename_component(FILE_EXT ${SOURCE_FILE} EXT)
		get_filename_component(FILE_DIR ${SOURCE_FILE} DIRECTORY)
		string(REPLACE ".hlsl" "" TARGET_NAME ${FILE_NAME})
		string(REPLACE "." "_" FILE_NAME_FORMATTED ${TARGET_NAME})
		string(TOUPPER ${FILE_NAME_FORMATTED} FILE_NAME_FORMATTED_UPPER)

		string(REPLACE "_hlsl" "" FUNCTION_NAME ${FILE_NAME_FORMATTED})

		# Set the compile target file.
		set(XENON_COMPILE_TARGET ${FILE_DIR}/${TARGET_NAME}.hpp)
		message(STATUS "Shader compile output set to ${XENON_COMPILE_TARGET}")

		# Setup the file containing the target file's content.
		file(
			WRITE 
			${XENON_COMPILE_TARGET} 

			"// Copyright 2022-2023 Nexonous\n"
			"// SPDX-License-Identifier: Apache-2.0\n"
			"\n"
			"#pragma once\n"
			"\n"
			"#include \"${CMAKE_SOURCE_DIR}/Engine/XenonBackend/Shader.hpp\"\n"
			"\n"
			"#include <cstdint>\n"
			"\n"
		)

		# Setup the variables for the output directory and output file.
		set(COMPILED_OUTPUT_DIR ${FILE_DIR}/Generated)
		set(OUTPUT_FILE_NAME ${FILE_NAME_WE}${FILE_EXT})
		set(OUTPUT_FILE ${COMPILED_OUTPUT_DIR}/${OUTPUT_FILE_NAME})

		# Comment or uncomment if we want to enable/ disable warnings.
		set(HIDE_WARNINGS -Wno-ignored-attributes)

		# Resolve the target profile.
		if (${SOURCE_FILE} MATCHES "vert")
			set_source_files_properties(${SOURCE_FILE} PROPERTIES ShaderType "vs")
			set(TARGET_PROFILE vs_6_6)
			set(SHADER_TYPE_MACRO "XENON_SHADER_VERTEX")

		elseif (${SOURCE_FILE} MATCHES "frag")
			set_source_files_properties(${SOURCE_FILE} PROPERTIES ShaderType "ps")
			set(TARGET_PROFILE ps_6_6)
			set(SHADER_TYPE_MACRO "XENON_SHADER_FRAGMENT")

		elseif (${SOURCE_FILE} MATCHES "comp")
			set_source_files_properties(${SOURCE_FILE} PROPERTIES ShaderType "cs")
			set(TARGET_PROFILE cs_6_6)
			set(SHADER_TYPE_MACRO "XENON_SHADER_COMPUTE")

		elseif (${SOURCE_FILE} MATCHES "geom")
			set_source_files_properties(${SOURCE_FILE} PROPERTIES ShaderType "gs")
			set(TARGET_PROFILE gs_6_6)
			set(SHADER_TYPE_MACRO "XENON_SHADER_GEOMETRY")

		elseif (${SOURCE_FILE} MATCHES "tesc")
			set_source_files_properties(${SOURCE_FILE} PROPERTIES ShaderType "hs")
			set(TARGET_PROFILE hs_6_6)
			set(SHADER_TYPE_MACRO "XENON_SHADER_TESSELLATION_CONTROL")

		elseif (${SOURCE_FILE} MATCHES "tese")
			set_source_files_properties(${SOURCE_FILE} PROPERTIES ShaderType "ds")
			set(TARGET_PROFILE ds_6_6)
			set(SHADER_TYPE_MACRO "XENON_SHADER_TESSELLATION_EVALUATION")
			
		elseif (${SOURCE_FILE} MATCHES "rgen")
			set_source_files_properties(${SOURCE_FILE} PROPERTIES ShaderType "lib")
			set(TARGET_PROFILE lib_6_6)
			set(SHADER_TYPE_MACRO "XENON_SHADER_RAY_GENERATION")

		elseif (${SOURCE_FILE} MATCHES "rint")
			set_source_files_properties(${SOURCE_FILE} PROPERTIES ShaderType "lib")
			set(TARGET_PROFILE lib_6_6)
			set(SHADER_TYPE_MACRO "XENON_SHADER_RAY_INTERSECTION")

		elseif (${SOURCE_FILE} MATCHES "rahit")
			set_source_files_properties(${SOURCE_FILE} PROPERTIES ShaderType "lib")
			set(TARGET_PROFILE lib_6_6)
			set(SHADER_TYPE_MACRO "XENON_SHADER_RAY_ANY_HIT")

		elseif (${SOURCE_FILE} MATCHES "rchit")
			set_source_files_properties(${SOURCE_FILE} PROPERTIES ShaderType "lib")
			set(TARGET_PROFILE lib_6_6)
			set(SHADER_TYPE_MACRO "XENON_SHADER_RAY_CLOSEST_HIT")

		elseif (${SOURCE_FILE} MATCHES "rmiss")
			set_source_files_properties(${SOURCE_FILE} PROPERTIES ShaderType "lib")
			set(TARGET_PROFILE lib_6_6)
			set(SHADER_TYPE_MACRO "XENON_SHADER_RAY_MISS")

		elseif (${SOURCE_FILE} MATCHES "rcall")
			set_source_files_properties(${SOURCE_FILE} PROPERTIES ShaderType "lib")
			set(TARGET_PROFILE lib_6_6)
			set(SHADER_TYPE_MACRO "XENON_SHADER_CALLABLE")

		else()
			set_source_files_properties(${SOURCE_FILE} PROPERTIES ShaderType "lib")
			set(TARGET_PROFILE lib_6_6)
			set(SHADER_TYPE_MACRO "XENON_SHADER_LIBRARY")

		endif()

		if(MSVC)
			# Specify DXIL the output file.
			set(DXIL_OUTPUT_FILE ${OUTPUT_FILE}.dxil.hpp)

			# Set the HLSL compile command.
			set(DXIL_COMPILE_COMMAND ${TOOLS_DIR}/dxc ${HIDE_WARNINGS} -Fh ${DXIL_OUTPUT_FILE} -I ${CMAKE_SOURCE_DIR}/Engine/XenonShaderBank -HV 2021 -Vn g_${FILE_NAME_FORMATTED_UPPER}_DXIL -T ${TARGET_PROFILE} -E main -D ${SHADER_TYPE_MACRO} ${SOURCE_FILE})

			# Add a custom command for the file to compile HLSL to SPIR-V.
			set_source_files_properties(${DXIL_OUTPUT_FILE} PROPERTIES SYMBOLIC TRUE)

			# Include the header file.
			file(
				APPEND 
				${XENON_COMPILE_TARGET} 

				"#include \"Generated/${OUTPUT_FILE_NAME}.dxil.hpp\"\n"
			)

		endif()

		# Specify SPIR-V the output file.
		set(SPIRV_OUTPUT_FILE ${OUTPUT_FILE}.spirv.hpp)

		# Set the SPIR-V compile command.
		# set(SPIRV_COMPILE_COMMAND ${TOOLS_DIR}/glslangValidator -V ${SOURCE_FILE} -o ${SPIRV_OUTPUT_FILE} --variable-name g_${FILE_NAME_FORMATTED_UPPER}_SPIRV --target-env vulkan1.3 -I${CMAKE_SOURCE_DIR}/Engine/ShaderBank --entry-point main)
		set(SPIRV_COMPILE_COMMAND ${TOOLS_DIR}/dxc ${HIDE_WARNINGS} -Fh ${SPIRV_OUTPUT_FILE} -I ${CMAKE_SOURCE_DIR}/Engine/XenonShaderBank -HV 2021 -Vn g_${FILE_NAME_FORMATTED_UPPER}_SPIRV -T ${TARGET_PROFILE} -fspv-target-env=vulkan1.3 -E main -D ${SHADER_TYPE_MACRO} -spirv ${SOURCE_FILE})

		# Add a custom command for the file to compile HLSL to SPIR-V.
		set_source_files_properties(${SPIRV_OUTPUT_FILE} PROPERTIES SYMBOLIC TRUE)

		# Include the header file.
		file(
			APPEND 
			${XENON_COMPILE_TARGET} 
			
			"#include \"Generated/${OUTPUT_FILE_NAME}.spirv.hpp\"\n"
			"\n"
		)

		# Setup the platform specific crate functions.
		if(MSVC)
			file(
				APPEND 
				${XENON_COMPILE_TARGET} 
				
				"namespace Xenon { namespace Generated { [[nodisacrd]] inline Xenon::Backend::Shader CreateShader${FUNCTION_NAME}() { return Xenon::Backend::Shader::Create(g_${FILE_NAME_FORMATTED_UPPER}_SPIRV, g_${FILE_NAME_FORMATTED_UPPER}_DXIL); } } }\n"
			)

		else()
			file(
				APPEND 
				${XENON_COMPILE_TARGET} 
				
				"namespace Xenon { namespace Generated { [[nodisacrd]] inline Xenon::Backend::Shader CreateShader${FUNCTION_NAME}() { return Xenon::Backend::Shader::Create(g_${FILE_NAME_FORMATTED_UPPER}_SPIRV); } } }\n"
			)

		endif()

		# Create the output directory.
		file(MAKE_DIRECTORY ${COMPILED_OUTPUT_DIR})

		# Compile the shader while configuring CMake.
		execute_process(
		    COMMAND ${DXIL_COMPILE_COMMAND}
		    COMMAND ${SPIRV_COMPILE_COMMAND}
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			COMMAND_ECHO STDOUT
			ERROR_VARIABLE SHADER_COMPILER_STATUS
		)

		# Show the process status.
		message(STATUS ${SHADER_COMPILER_STATUS})
		set(SHADER_SOURCE_ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE})

		# Add the custom command to compile the shader.
		add_custom_command(
		    OUTPUT ${DXIL_OUTPUT_FILE} ${SPIRV_OUTPUT_FILE}
			COMMAND ${CMAKE_COMMAND} -E make_directory ${COMPILED_OUTPUT_DIR}
		    COMMAND ${DXIL_COMPILE_COMMAND}
		    COMMAND ${SPIRV_COMPILE_COMMAND}
		    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			DEPENDS ${SHADER_SOURCE_ABSOLUTE} ${XENON_COMPILE_TARGET}
			COMMENT "Compiling ${SOURCE_FILE}"
			VERBATIM
		)

		# Make sure dependencies are right.
		set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${SHADER_SOURCE_ABSOLUTE})
		configure_file(${SHADER_SOURCE_ABSOLUTE} ${SHADER_SOURCE_ABSOLUTE} COPYONLY)

		# Stop VS from trying to compile HLSL files.
		set_source_files_properties(${SOURCE_FILE} PROPERTIES VS_TOOL_OVERRIDE "None")

	endforeach()

endfunction()