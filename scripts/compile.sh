#!/usr/bin/env bash
# Copyright PA Knowledge Ltd 2021
# MIT License. For licence terms see LICENCE.md file.

set -eu

BUILD_TYPE=${1}
BUILD_TYPE_LOWERCASE="${BUILD_TYPE,,}"
BUILD_TARGET=${2:-"docker"}
BUILD_FOLDER=cmake-build-"$BUILD_TARGET"-"$BUILD_TYPE_LOWERCASE"

mkdir -p "$BUILD_FOLDER"
pushd "$BUILD_FOLDER"

cmake3 -DCMAKE_BUILD_TYPE="$BUILD_TYPE" ..
make -j 8

popd
