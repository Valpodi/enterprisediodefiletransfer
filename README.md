# Enterprise Diode Tester source code


Compile the code for a centos 7 host using docker:

    ./scripts/compileInDocker.sh release

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
2. ADDRESS: The IP address of the server.
3. PORT: The port to send from.
4. MTUSIZE: The size of the mtu in bytes.
5. DATARATE_MBPS: The desired datarate in megabytes per second. Defaults to 0 - which means