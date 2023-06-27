// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <version>

// Macro to check if a feature is available by comparing it to an expected value.
#define XENON_CHECK_FEATURE(feature, value)	feature >= value

// Check and define the XENON_FEATURE_SOURCE_LOCATION macro if the std::source_location class is available.
#ifdef __cpp_lib_source_location
#	if XENON_CHECK_FEATURE(__cpp_lib_source_location, 201907L)
// The compiler supports the' __cpp_lib_source_location' feature (has support for std::source_location).
#		define XENON_FEATURE_SOURCE_LOCATION
#	endif
#endif

// Check and define the XENON_FEATURE_RANGES macro if the std::ranges namespace is available.
#ifdef __cpp_lib_ranges
#	if XENON_CHECK_FEATURE(__cpp_lib_ranges, 201911L)
// The compiler supports the' __cpp_lib_ranges' feature (has support for std::ranges).
#		define XENON_FEATURE_RANGES
#	endif
#endif

// Check and define the XENON_FEATURE_BIT_CAST macro if the std::ranges namespace is available.
#ifdef __cpp_lib_bit_cast
#	if XENON_CHECK_FEATURE(__cpp_lib_bit_cast, 201806L)
// The compiler supports the' __cpp_lib_bit_cast' feature (has support for std::bit_cast).
#		define XENON_FEATURE_BIT_CAST
#	endif
#endif