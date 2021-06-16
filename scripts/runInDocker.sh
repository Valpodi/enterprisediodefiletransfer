#!/usr/bin/env bash
# Copyright PA Knowledge Ltd 2021
# MIT License. For licence terms see LICENCE.md file.

set -eux

DOCKERCOMMAND=${1:-"exit 1"}
BUILDIMAGE=$(docker build -q -f docker/Dockerfile .)


docker run -v "$(pwd)":"$(pwd)" \
          "$BUILDIMAGE" \
          /bin/bash -c "cd $(pwd) && $DOCKERCOMMAND"

