#!/usr/bin/env bash
# Copyright PA Knowledge Ltd 2021
# MIT License. For licence terms see LICENCE.md file.

set -eux

echo "***************************************"
echo "*     Running Code QL                *"
echo "***************************************"

rm -rf cmake-codeql
mkdir cmake-codeql
pushd cmake-codeql
scl enable devtoolset-9 "cmake3 .."
scl enable devtoolset-9 "/codeql/codeql database create oakdoor-database --language=cpp --command  make"