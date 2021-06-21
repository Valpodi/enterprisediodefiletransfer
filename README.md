<p align="center"><img width=12.5% src="https://avatars.githubusercontent.com/u/49148719?s=100&v=4"></p>

# Oakdoor Enterprise Data Diode File Transfer

![CI build](https://github.com/oakdoor/enterprisediodefiletransfer/actions/workflows/build.yml/badge.svg)

This repository contains the source code and scripts necessary to build and run a udp file transfer client and server. This is designed for file transfer across the  [Oakdoor](https://oakdoor.io/) Enterprise Data Diodes, including the Import and Basic variants.

The "tester" is a loopback application running a server and client simultaneously.

## Application Notes:
* Sending and receiving a file using non-acknowledged UDP packets
* The server application can receive multiple files multiplexed on a single receive UDP port.
* Re-ordering of UDP packets.
* Lost packets are not recovered and result in the file being lost
* The filename is encoded in a SISL (https://pypi.org/project/pysisl/) UDP message to support file transfer over the Oakdoor Enterprise Diode Import variant, which provides UDP frame level syntax verification and encapsulation.

## Centos 7 build instructions
The following instructions compile the file transfer application for Centos 7 like targets. For other Linux targets, depending on the versions of dependencies such as libc++, it may be necessary to compile the code on your platform. 

Compile the code and run the tests using docker:

    ./scripts/compileAndTestInDocker.sh release

The server, client and tester binaries may be found in the cmake-build-docker-release folder.

Optionally: Check the binaries are working by running the integration tests:

    ./scripts/runIntegrationTests.sh

## Building on other linux targets
These are basic notes on the cmake build process. It may be necessary to dependencies appropriately, for example boost.

    mkdir cmake-build
    cd cmake-build
    camke -DCMAKE_BUILD_TYPE=release ..
    make

The server, client and tester binaries may be found in the cmake-build folder.

## Sending and receiving files

### Catcher
On the receiving PC (the "catcher"), start the server application:

     ./server [-s PORT] [-m MTUSIZE] [-q QUEUELENGTH] [-i]

      -s, --serverPort PORT
            Specifies the UDP port the server will listen on. Default value of 45000.
      -m, --mtu MTUSIZE
            Network MTU size. Default size of 1500.
      -q, --queueLength QUEUELENGTH
            The number of packets to queue in the case of missing / out of order packets. Default 1024 packets.
      -i, --importDiode
            Set this parameter if using the Oakdoor Enterprise Import Diode. This will re-wrap encapsulated files with a single key.
      -l, --logLevel
            Logging level for program output. Default level is info.

### Pitcher
On the sending PC (the "pitcher"), send the file:
    
      ./client -f FILENAME -a ADDRESS -c PORT [--mtu MTUSIZE] [--datarate DATARATE_MBPS]

      -f, --filename FILENAME
         Path of file to send. Note that the maximum length of the filename (not the path) is 65 characters, and the filename can only contain alphanumeric characters, dashes(-) and dots(.). Only the filename is sent to the destination. Parent folders are not reconstructed.
      -a, --address ADDRESS
         Target address of the UDP server or diode.
      -c, --clientPort PORT
         Target UDP Port.
      -m, --mtu MTUSIZE
         Size of the MTU in bytes
      -r, --datarate DATARATE
         The desired datarate in megabytes per second. Defaults to 0 (as fast as possible)
      -l, --logLevel
            Logging level for program output. Default level is info.

Or if running the loopback tester:

    ./tester -f FILENAME -a ADDRESS -c CLIENTPORT -s SERVERPORT [-m MTUSIZE] [--datarate DATARATE_MBPS] [-q reorder_packet_queue_size] [-i]

      -f, --filename FILENAME
            Path of file to send. Note that the maximum length of the filename (not the path) is 65 characters, and the filename can only contain alphanumeric characters, dashes(-) and dots(.). Only the filename is sent to the destination. Parent folders are not reconstructed.
      -a, --address ADDRESS
            Target address of the UDP server or diode.
      -c, --clientPort PORT
            Target UDP Port.
      -s, --serverPort PORT
            Specifies the UDP port the server will listen on.
      -m, --mtu MTUSIZE
            Network MTU size. Default size of 1500.
      -q, --queueLength QUEUELENGTH
            The number of packets to queue in the case of missing / out of order packets. Default 1024 packets.
      -i, --importDiode
            Set this parameter if using the Oakdoor Enterprise Import Diode. This will re-wrap encapsulated files with a single ke
      -r, --datarate DATARATE
         The desired datarate in megabytes per second. Defaults to 0 (as fast as possible)
      -l, --logLevel
            Logging level for program output. Default level is info.


## CHANGELOG

### v1.0.6
wip - supporting the Import Diode and re-wrapping encapsulated file with the same key.
 

### v1.0.5
Supporting transfer of the file filename with a restricted character set.