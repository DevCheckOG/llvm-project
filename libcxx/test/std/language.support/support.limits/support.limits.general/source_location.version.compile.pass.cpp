//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// WARNING: This test was generated by generate_feature_test_macro_components.py
// and should not be edited manually.

// <source_location>

// Test the feature test macros defined by <source_location>

// clang-format off

#include <source_location>
#include "test_macros.h"

#if TEST_STD_VER < 14

#  ifdef __cpp_lib_source_location
#    error "__cpp_lib_source_location should not be defined before c++20"
#  endif

#elif TEST_STD_VER == 14

#  ifdef __cpp_lib_source_location
#    error "__cpp_lib_source_location should not be defined before c++20"
#  endif

#elif TEST_STD_VER == 17

#  ifdef __cpp_lib_source_location
#    error "__cpp_lib_source_location should not be defined before c++20"
#  endif

#elif TEST_STD_VER == 20

#  ifndef __cpp_lib_source_location
#    error "__cpp_lib_source_location should be defined in c++20"
#  endif
#  if __cpp_lib_source_location != 201907L
#    error "__cpp_lib_source_location should have the value 201907L in c++20"
#  endif

#elif TEST_STD_VER == 23

#  ifndef __cpp_lib_source_location
#    error "__cpp_lib_source_location should be defined in c++23"
#  endif
#  if __cpp_lib_source_location != 201907L
#    error "__cpp_lib_source_location should have the value 201907L in c++23"
#  endif

#elif TEST_STD_VER > 23

#  ifndef __cpp_lib_source_location
#    error "__cpp_lib_source_location should be defined in c++26"
#  endif
#  if __cpp_lib_source_location != 201907L
#    error "__cpp_lib_source_location should have the value 201907L in c++26"
#  endif

#endif // TEST_STD_VER > 23

// clang-format on
