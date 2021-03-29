#!/usr/bin/bash
# Copyright PA Knowledge Ltd 2021
# For licence terms see LICENCE.md file

set -eux

BUILDTYPE=${1}

ARCHIVE=${2:-"no"}

BUILDIMAGE=$(docker build -q -f docker/Dockerfile .)

docker run -v "$(pwd)":"$(pwd)" \
  "$BUILDIMAGE" \
  /bin/bash -c "pushd $(pwd) && scl enable devtoolset-9 \"HOME=/tmp ./scripts/compile.sh ${BUILDTYPE}\" && ./scripts/runIntegrationTests.sh && chown -R $(id -u):$(id -u) ."

if [[ ${ARCHIVE} == "archive" ]]; then
  pushd cmake-build-release
    zip ../FileTransferApp.zip server client tester
  popd
fi
