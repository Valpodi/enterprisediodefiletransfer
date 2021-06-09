#!/usr/bin/env bash
# Copyright PA Knowledge Ltd 2021
# MIT License. For licence terms see LICENCE.md file.

set -eux

BUILD_FOLDER=${1:-cmake-build-release}

rm -f IntegrationTestResults*.xml

./"$BUILD_FOLDER"/UnitTests [integration] -r junit -o IntegrationTestResultsCpp.xml

BUILD_FOLDER=$BUILD_FOLDER python3 -m nose --with-xunit --xunit-file=IntegrationTestResultsPython.xml --nologcapture --nocapture -v -x \
        scripts/integration_tests.py
