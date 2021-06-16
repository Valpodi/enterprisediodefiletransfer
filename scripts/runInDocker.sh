#!/usr/bin/env bash
# Copyright PA Knowledge Ltd 2021
# MIT License. For licence terms see LICENCE.md file.

set -eux

DOCKERCOMMAND=${1:-"exit 1"}
BUILDIMAGE=$(docker build -q -f docker/Dockerfile .)

function cleanup()
{
  echo "cleanup! reset all file permissions to host owned"
  HOSTUID=`id -u`
  docker run -v $(pwd):$(pwd) "$BUILDIMAGE" /bin/bash -c "chown -R $HOSTUID $(pwd)"
}

trap cleanup EXIT


docker run -v "$(pwd)":"$(pwd)" \
          "$BUILDIMAGE" \
          /bin/bash -c "cd $(pwd) && $DOCKERCOMMAND"

