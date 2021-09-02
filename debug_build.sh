#! /usr/bin/bash

set -e

cd ~/src/enterprisediodefiletransfer/
./scripts/compile.sh Debug

cd cmake-build-docker-debug
scp -o "ProxyJump user@oakdoor-bastion" server user@ed-tester-4-high-1.oakdoor.ed:./software/server
scp -o "ProxyJump user@oakdoor-bastion" client user@ed-tester-4-low-1.oakdoor.ed:./software/client

