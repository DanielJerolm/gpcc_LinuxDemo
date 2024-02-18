# GPCC Linux Demo
#
# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file,
# You can obtain one at https://mozilla.org/MPL/2.0/.
#
# Copyright (C) 2024 Daniel Jerolm

# ===================================================================
# Sub-CMakeLists-file for gpcc_LinuxDemo.
# Builds the project's artifacts for the unittest target environment.
# ===================================================================


# Note: Current working directory is gpcc_LinuxDemo's root directory

include(extern/toolchain_configs/native_gcc/settings.x86_64-any-linux.cmake)

# ---------------------------------------------------------------------------------------------------------------------
# Exclusion of selected GPCC unit test cases
# ---------------------------------------------------------------------------------------------------------------------
# By default, the unit test executable will include all of GPCC's unit tests, even those that depend on CPU load. To
# achieve reproducable results (e.g. on a build-server) you have to exclude at least those tests which depend on CPU
# load.
# Limited user rights and limited resources may also require exclusion of tests requiring special user rights or an
# huge amount of memory.

set(GPCC_SkipTFCBasedTests           OFF CACHE BOOL "" FORCE)
set(GPCC_SkipLoadDependentTests      OFF CACHE BOOL "" FORCE)
set(GPCC_SkipVeryBigMemTests         OFF CACHE BOOL "" FORCE)
set(GPCC_SkipSpecialRightsBasedTests OFF CACHE BOOL "" FORCE)

# ---------------------------------------------------------------------------------------------------------------------
# Add and configure sub-projects
# ---------------------------------------------------------------------------------------------------------------------
# --- demo_application ---
set(BUILD_UNITTEST ON CACHE BOOL "" FORCE)
add_subdirectory(demo_application)

# --- googletest ---
set(INSTALL_GTEST OFF CACHE BOOL "Prevent installation of googletest" FORCE)
add_subdirectory(extern/googletest)

# --- GPCC ---
set(GPCC_TargetEnvironment "unittest" CACHE STRING "" FORCE)

if((CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "amd64"))
  set(GPCC_Compiler "gcc_x64" CACHE STRING "" FORCE)
  set(GPCC_OS "linux_x64_tfc" CACHE STRING "" FORCE)
elseif((CMAKE_SYSTEM_PROCESSOR STREQUAL "arm" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64"))
  set(GPCC_Compiler "gcc_arm" CACHE STRING "" FORCE)
  set(GPCC_OS "linux_arm_tfc" CACHE STRING "" FORCE)
else()
  message(FATAL_ERROR "System processor (${CMAKE_SYSTEM_PROCESSOR}) not supported. Supported processors:\n"
                      "x86_64, amd64, arm, arm64")
endif()

add_subdirectory(extern/gpcc)

# ---------------------------------------------------------------------------------------------------------------------
# Artifact: Unit test executable
# ---------------------------------------------------------------------------------------------------------------------
add_executable(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
                      PRIVATE
                      demo_application_testcases
                      gpcc_testcases
                      gmock_main)

set_target_properties(${PROJECT_NAME} PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output"
                      OUTPUT_NAME "unittests"
                     )
