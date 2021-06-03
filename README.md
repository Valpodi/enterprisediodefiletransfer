# Enterprise Diode file transfer source code

This repository contains the source code and scripts necessary to build and run a udp file transfer client and server.
The "tester" is a loopback application running a server and client simultaneously.

Compile the code and run the tests using docker:

    ./scripts/compileAndTestInDocker.sh release

Optionally: Check the binaries are working by running the integration tests:

    ./scripts/runIntegrationTests.sh

On the pitcher and catcher run the client and server manually:

    ./server --serverPort PORT --mtu MTUSIZE --queueLength QUEUELENGTH
    ./client --filename FILENAME --address ADDRESS --clientPort PORT --mtu MTUSIZE --datarate DATARATE_MBPS

Server arguments:
1. PORT: The port the server will listen on.
2. MTUSIZE: The size of the mtu in bytes.
3. QUEUELENGTH: The number of packets to queue in the case of missing / out of order packets.

Client arguments:
1. FILENAME: The path to the file to send.
   
   Note that the maximum length of the filename (not the path) is 65 characters, and the filename can only contain alphanumeric characters, dashes(-) and dots(.).
2. ADDRESS: The IP address of the server.
3. PORT: The port to send from.
4. MTUSIZE: The size of the mtu in bytes.
5. DATARATE_MBPS: The desired datarate in megabytes per second. Defaults to 0 (as fast as possible)

Or if running the loopback tester:

    ./tester --filename FILENAME --address ADDRESS --clientPort PORT --serverPort PORT --mtu MTUSIZE --datarate DATARATE_MBPS

Client arguments:
1. FILENAME: The path to the file to send.

   Note that the maximum length of the filename (not the path) is 65 characters, and the filename can only contain alphanumeric characters, dashes(-) and dots(.).
2. ADDRESS: The IP address of the server.
3. CLIENT PORT: The port to send from.
4. SERVER PORT: The port the server will listen on.
5. MTUSIZE: The size of the mtu in bytes.
6. DATARATE_MBPS: The desired datarate in megabytes per second. Defaults to 0 (as fast as possible)


