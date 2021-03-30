#!/usr/bin/env bash
# Copyright PA Knowledge Ltd 2021
# MIT License. For licence terms see LICENCE.md file.

set -eux

rm -f IntegrationTestResults.xml
rm -f EDTesterIntegrationTestResults.xml

./cmake-build-release/UnitTests [integration] -r junit -o IntegrationTestResults.xml

python3 -m nose --with-xunit --xunit-file=EDTesterIntegrationTestResults.xml --nologcapture --nocapture -v -x \
        scripts/integration_tests.py