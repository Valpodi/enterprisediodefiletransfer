#!/usr/bin/env bash
# Copyright PA Knowledge Ltd 2021
# MIT License. For licence terms see LICENCE.md file.

set -eux

BUILD_FOLDER=${1:-cmake-build-release}

echo "***************************************"
echo "*     Running Valgrind                *"
echo "***************************************"

valgrind  --tool=memcheck\
          --gen-suppressions=all \
          --leak-check=full \
          --leak-resolution=med \
          --track-origins=yes \
          --vgdb=no \
          --error-exitcode=200 \
          "$BUILD_FOLDER"/UnitTests \
          -d yes \
          --order lex \
          ~[integration]
