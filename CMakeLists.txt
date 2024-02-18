# GPCC Linux Demo
#
# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file,
# You can obtain one at https://mozilla.org/MPL/2.0/.
#
# Copyright (C) 2024 Daniel Jerolm

# ================================================
# Top-level CMakeLists.txt-file for gpcc_LinuxDemo
# ================================================
#
# System requirements
# -------------------
# - Build environment: Linux host
# - Target environment for artifcats: Linux host
# - Target environment CPU: x86_64, amd64, arm, or arm64
# - Toolchain: Linux host's native GCC
#
# Target environment
# ------------------
# During initial cmake configuration, you have to select the target environment and set the cache variable
# "TargetEnvironment" accordingly. Depending on the selected target environment, different artifacts will be build:
#
# - target environment "productive":
#     - The demo application will be build (build_productive/output/demoApp).
#     - GPCC is configured to provide an productive OSAL.
#
#     The native gcc toolchain will be used. No toolchain file must be provided when configuring the project.
#     Example:
#     cmake -S . \
#           -B build_productive \
#           -DTargetEnvironment:STRING=productive \
#           -DCMAKE_BUILD_TYPE=custom_release
#
# - target environment "unittest":
#     - An executable containing GPCC's unit test cases and the demo application's unit test cases will be build
#       (build_unittest/output/unittests)
#     - GPCC is configured to provide a non-productive OSAL implementing TFC.
#
#     The native gcc toolchain will be used. No toolchain file must be provided when configuring the project.
#     Example:
#     cmake -S . \
#           -B build_unittest \
#           -DTargetEnvironment:STRING=unittest \
#           -DCMAKE_BUILD_TYPE=custom_release
#
cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(gpcc_LinuxDemo)

# ---------------------------------------------------------------------------------------------------------------------
# User setting "TargetEnvironment"
# ---------------------------------------------------------------------------------------------------------------------
set(TargetEnvironment CACHE STRING "Selects the target environment.")
set(TargetEnvironment_Values "productive;unittest")

string(REPLACE ";"  ", " TargetEnvironment_Values_str "${TargetEnvironment_Values}")
set_property(CACHE TargetEnvironment PROPERTY STRINGS ${TargetEnvironment_Values})

if(NOT TargetEnvironment)
  message(FATAL_ERROR "Error: 'TargetEnvironment' is not set.\n"
                      "Allowed values: ${TargetEnvironment_Values_str}")
endif()
if(NOT (${TargetEnvironment} IN_LIST TargetEnvironment_Values))
  message(FATAL_ERROR "Error: 'TargetEnvironment' has invalid value.\n"
                      "Allowed values: ${TargetEnvironment_Values_str}")
endif()

# ---------------------------------------------------------------------------------------------------------------------
# Include the cmake file which is specific to the target environment
# ---------------------------------------------------------------------------------------------------------------------
include(cmake/CMakeLists.${TargetEnvironment}.cmake)
