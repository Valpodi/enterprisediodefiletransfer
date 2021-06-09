#!/usr/bin/bash
# Copyright PA Knowledge Ltd 2021
# MIT License. For licence terms see LICENCE.md file.

set -eux

BUILD_TYPE=${1}
BUILD_TYPE_LOWERCASE="${BUILD_TYPE,,}"
BUILD_TARGET="docker"
BUILD_FOLDER=cmake-build-"$BUILD_TARGET"-"$BUILD_TYPE_LOWERCASE"
BUILD_IMAGE=$(docker build -q -f docker/Dockerfile .)
ARCHIVE=${2:-"no"}

docker run -v "$(pwd)":"$(pwd)" \
  "$BUILD_IMAGE" \
  /bin/bash -c "pushd $(pwd) && scl enable devtoolset-9 \"HOME=/tmp ./scripts/compile.sh $BUILD_TYPE $BUILD_TARGET\" && ./scripts/runIntegrationTests.sh $BUILD_FOLDER && chown -R $(id -u):$(id -u) ."

if [[ ${ARCHIVE} == "archive" ]]; then
  pushd cmake-build-docker-release
    zip ../FileTransferApp.zip server client tester
  popd
fi
