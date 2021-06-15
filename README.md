# Oakdoor Enterprise Data Diode File Transfer Application

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

On the receiving PC (the "catcher"), start the server application:

    ./server --serverPort PORT --mtu MTUSIZE --queueLength QUEUELENGTH --importDiode IMPORTDIODE

Server arguments:
1. PORT: The port the server will listen on.
2. MTUSIZE: The size of the mtu in bytes.
3. QUEUELENGTH: The number of packets to queue in the case of missing / out of order packets.
3. IMPORTDIODE: default=false, set to true when using an import diode, so that the data gets re-wrapped.

On the sending PC (the "pitcher"), send the file:
    
    ./client --filename FILENAME --address ADDRESS --clientPort PORT --mtu MTUSIZE --datarate DATARATE_MBPS

Client arguments:
1. FILENAME: The path to the file to send.
   
   Note that the maximum length of the filename (not the path) is 65 characters, and the filename can only contain alphanumeric characters, dashes(-) and dots(.).
2. ADDRESS: The IP address of the server.
3. PORT: The port to send from.
4. MTUSIZE: The size of the mtu in bytes.
5. DATARATE_MBPS: The desired datarate in megabytes per second. Defaults to 0 (as fast as possible)

Or if running the loopback tester:

    ./tester --filename FILENAME --address ADDRESS --clientPort PORT --serverPort PORT --mtu MTUSIZE --datarate DATARATE_MBPS --importDiode IMPORTDIODE

Client arguments:
1. FILENAME: The path to the file to send.

   Note that the maximum length of the filename (not the path) is 65 characters, and the filename can only contain alphanumeric characters, dashes(-), dots(.) and underscore (_).
2. ADDRESS: The IP address of the server.
3. CLIENT PORT: The port to send from.
4. SERVER PORT: The port the server will listen on.
5. MTUSIZE: The size of the mtu in bytes.
6. DATARATE_MBPS: The desired datarate in megabytes per second. Defaults to 0 (as fast as possible)



## CHANGELOG

### v1.0.6
wip - supporting the Import Diode and re-wrapping encapsulated file with the same key.
 

### v1.0.5
Supporting transfer of the file filename with a restricted character set.