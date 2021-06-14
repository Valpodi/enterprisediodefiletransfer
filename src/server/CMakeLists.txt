#Copyright PA Knowledge Ltd 2021
#MIT License. For licence terms see LICENCE.md file.

add_library(SERVER_LIBRARY
        UdpServer.cpp
        UdpServerInterface.hpp
        OrderingStreamWriter.cpp
        ReorderPackets.cpp
        Server.cpp
        SessionManager.cpp
        FileStream.hpp
        StreamInterface.hpp
        Packet.cpp
        DropStream.hpp
        SISLFilename.cpp
        SISLFilename.hpp)

add_library(SERVER_LIBRARY_TESTS
        ../test/EnterpriseDiodeTestHelpers.cpp
        ServerTests.cpp
        SessionManagerTests.cpp
        UdpServerTests.cpp
        ReorderPacketsTests.cpp
        OrderingStreamWriterTests.cpp
        StreamSpy.hpp)
