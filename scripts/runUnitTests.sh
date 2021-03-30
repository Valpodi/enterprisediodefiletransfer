#!/usr/bin/env bash
# Copyright PA Knowledge Ltd 2021
# MIT License. For licence terms see LICENCE.md file.

set -eux

rm -f UnitTestResults.xml

./cmake-build-debug/UnitTests ~[integration] -r junit -o UnitTestResults.xml
