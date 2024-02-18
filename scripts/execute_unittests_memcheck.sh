#!/bin/bash

# GPCC Linux Demo
#
# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
# If a copy of the MPL was not distributed with this file,
# You can obtain one at https://mozilla.org/MPL/2.0/.
#
# Copyright (C) 2024 Daniel Jerolm

set -e

# check number of arguments
if [ $# -ne 1 ]; then
  echo "Exactly one argument expected: <gtest filter>"
  echo "Example: ./execute_unittests_memcheck.sh demo_application_tests*"
  exit 1
fi

cd ../build_unittest/output
valgrind --tool=memcheck ./unittests --gtest_filter=$1:-*Death*
