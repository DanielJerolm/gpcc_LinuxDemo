# GPCC Linux Demo
#
# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file,
# You can obtain one at https://mozilla.org/MPL/2.0/.
#
# Copyright (C) 2024 Daniel Jerolm

# ===========================================================
# Main CMakeLists.txt-file for sub-project "demo_application"
# ===========================================================
#
# Dependencies that must be supplied by the top-project:
# - BUILD_UNITTEST == OFF
#   - GPCC
#
# - BUILD_UNITTEST == ON
#   - GPCC
#   - googletest
#
# Artifacts build by this project:
# - BUILD_UNITTEST == OFF
#   - Static library demo_application
#
# - BUILD_UNITTEST == ON
#   - Static library demo_application
#   - Object library demo_application_testcases
#

cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(demo_application)

# ---------------------------------------------------------------------------------------------------------------------
# Settings
# ---------------------------------------------------------------------------------------------------------------------
set(BUILD_UNITTEST OFF CACHE BOOL "Controls if the top project builds the unittests (ON) or the productive executable (OFF)")


function(SetRequiredCompilerOptionsAndFeatures target)
  target_compile_features(${target} PUBLIC cxx_std_17)

  if(CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(${target} PUBLIC "$<$<COMPILE_LANGUAGE:CXX>:-fexceptions>")
    target_compile_options(${target} PUBLIC "$<$<COMPILE_LANGUAGE:CXX>:-frtti>")

    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 8.0)
      # reduce length of file-names in debug messages contained in the libs
      target_compile_options(${target} PRIVATE -fmacro-prefix-map=${PROJECT_SOURCE_DIR}/=demo_application/)
    endif()
  else()
    message(FATAL_ERROR "Error: Compiler in use is not supported.")
  endif()
endfunction()


# ---------------------------------------------------------------------------------------------------------------------
# Target: demo_application library
# ---------------------------------------------------------------------------------------------------------------------
add_library(${PROJECT_NAME})

SetRequiredCompilerOptionsAndFeatures(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PRIVATE .)
target_link_libraries(${PROJECT_NAME} PRIVATE gpcc)

add_subdirectory(src)

if(NOT BUILD_UNITTEST)
  add_subdirectory(src_notest)
elseif()
  add_subdirectory(test_src)
endif()

# ---------------------------------------------------------------------------------------------------------------------
# Target: demo_application_testcases object library
# ---------------------------------------------------------------------------------------------------------------------
if(BUILD_UNITTEST)
  add_library(${PROJECT_NAME}_testcases OBJECT)

  SetRequiredCompilerOptionsAndFeatures(${PROJECT_NAME}_testcases)
  target_include_directories(${PROJECT_NAME}_testcases PRIVATE .)
  target_link_libraries(${PROJECT_NAME}_testcases PRIVATE ${PROJECT_NAME} gpcc gmock)

  add_subdirectory(testcases)
endif()
