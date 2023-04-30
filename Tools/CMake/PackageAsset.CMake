# Copyright 2022-2023 Dhiraj Wishal
# SPDX-License-Identifier: Apache-2.0

# Main build script for Xenon.
# Set the minimum required CMake version.
cmake_minimum_required(VERSION 3.22.2)

# PackageAsset function.
# This function can be used to package assets by using an input JSON file which describes where the assets are placed, and which also contains other information.
# The output file name will be the input file name appended by .xpkg.
# For example, if the INPUT_FILE is given as "example.json", then the output file name will be "example.xpkg"
#
# @param INPUT_FILE The input file descriptor.
# @param OUTPUT_DIRECTORY The output file directory.
function(PackageAsset PACKAGE_TARGET INPUT_FILE OUTPUT_DIRECTORY)
	get_filename_component(INPUT_FILE_NAME ${INPUT_FILE} NAME_WE)
	set(OUTPUT_FILE ${OUTPUT_DIRECTORY}/${INPUT_FILE_NAME}.xpkg)
	
	# Add the command.
	add_custom_command(
		TARGET ${PACKAGE_TARGET}
		POST_BUILD

		COMMAND $<TARGET_FILE:XenonAssetPackager> ${INPUT_FILE} ${OUTPUT_FILE}
		BYPRODUCTS ${OUTPUT_FILE}

		COMMENT Packaging ${INPUT_FILE} using the Xenon Asset Packager...
	)

endfunction()

# PackageAssets function.
# This function can be used to package multiple asset definitions by using an input JSON file which describes where the assets are placed, and which also contains other information.
# The output file name will be the input file name appended by .xpkg.
# For example, if the INPUT_FILE is given as "example.json", then the output file name will be "example.xpkg"
#
# @param INPUT_FILES The input file descriptors.
# @param OUTPUT_DIRECTORY The output file directory.
function(PackageAssets PACKAGE_TARGET INPUT_FILES OUTPUT_DIRECTORY)
	foreach(INPUT_FILE IN LISTS INPUT_FILES)
		get_filename_component(INPUT_FILE_NAME ${INPUT_FILE} NAME_WE)
		set(OUTPUT_FILE ${OUTPUT_DIRECTORY}/${INPUT_FILE_NAME}.xpkg)
	
		# Add the command.
		add_custom_command(
			TARGET ${PACKAGE_TARGET}
			POST_BUILD

			COMMAND $<TARGET_FILE:XenonAssetPackager> ${INPUT_FILE} ${OUTPUT_FILE}
			BYPRODUCTS ${OUTPUT_FILE}

			COMMENT Packaging ${INPUT_FILE} using the Xenon Asset Packager...
		)

	endforeach()

endfunction()