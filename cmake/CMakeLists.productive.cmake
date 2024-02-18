# GPCC Linux Demo
#
# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file,
# You can obtain one at https://mozilla.org/MPL/2.0/.
#
# Copyright (C) 2024 Daniel Jerolm

# =====================================================================
# Sub-CMakeLists-file for gpcc_LinuxDemo.
# Builds the project's artifacts for the productive target environment.
# =====================================================================


# Note: Current working directory is gpcc_dev's root directory

include(extern/toolchain_configs/native_gcc/settings.x86_64-any-linux.cmake)

# ---------------------------------------------------------------------------------------------------------------------
# Add and configure sub-projects
# ---------------------------------------------------------------------------------------------------------------------
# --- demo_application ---
set(BUILD_UNITTEST OFF CACHE BOOL "" FORCE)
add_subdirectory(demo_application)

# --- GPCC ---
set(GPCC_TargetEnvironment "productive" CACHE STRING "" FORCE)

if((CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "amd64"))
  set(GPCC_Compiler "gcc_x64" CACHE STRING "" FORCE)
  set(GPCC_OS "linux_x64" CACHE STRING "" FORCE)
elseif((CMAKE_SYSTEM_PROCESSOR STREQUAL "arm" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64"))
  set(GPCC_Compiler "gcc_arm" CACHE STRING "" FORCE)
  set(GPCC_OS "linux_arm" CACHE STRING "" FORCE)
else()
  message(FATAL_ERROR "System processor (${CMAKE_SYSTEM_PROCESSOR}) not supported. Supported processors:\n"
                      "x86_64, amd64, arm, arm64")
endif()

add_subdirectory(extern/gpcc)

# ---------------------------------------------------------------------------------------------------------------------
# Artifact: Demo application
# ---------------------------------------------------------------------------------------------------------------------
add_executable(${PROJECT_NAME} empty.cpp)

target_link_libraries(${PROJECT_NAME}
                      PRIVATE
                      demo_application)

set_target_properties(${PROJECT_NAME} PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output"
                      OUTPUT_NAME "demoApp"
                     )
