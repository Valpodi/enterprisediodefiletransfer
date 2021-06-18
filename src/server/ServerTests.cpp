// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <cstdint>
#include <vector>
#include <string>

#include "test/catch.hpp"
#include "test/EnterpriseDiodeTestHelpers.hpp"
#include "diodeheader/EnterpriseDiodeHeader.hpp"
#include "Server.hpp"
#include "StreamSpy.hpp"

TEST_CASE("ED server.")
{
  std::stringstream outputStream;
  std::uint32_t capturedSessionId = 0;
  boost::asio::io_service fake_service;

  Server edServer = createEdServer(std::make_unique<UdpServerFake>(0, fake_service, 0, 0), 16, 100,
                                   capturedSessionId, outputStream, DiodeType::basic);


  SECTION("A packet received by the server is written to the stream")
  {
    edServer.receivePacket(createTestPacketStream(1, 1, false), {'B'});

    REQUIRE(outputStream.str() == std::string("B"));

    const std::string filename = "{name: !str \"testFilename\"}";
    edServer.receivePacket(createTestPacketStream(1, 2, true), {filename.begin(), filename.end()});

    REQUIRE(outputStream.str() == std::string("B"));
  }

  SECTION("Packets received are inserted into the same stream until the EOF flag is raised")
  {
    edServer.receivePacket(createTestPacketStream(1, 1, false), {'B', 'C'});

    REQUIRE(outputStream.str() == std::string("BC"));

    edServer.receivePacket(createTestPacketStream(1, 2, false), {'D'});

    REQUIRE(outputStream.str() == std::string("BCD"));
    std::string filename = "{name: !str \"testFilename\"}";
    edServer.receivePacket(createTestPacketStream(1, 3, true), {filename.begin(), filename.end()} );

    REQUIRE(outputStream.str() == std::string("BCD"));
  }

  SECTION("Packets are reordered before being written to stream")
  {
    edServer.receivePacket(createTestPacketStream(1, 2, false), {'C', 'D'});

    REQUIRE(outputStream.str() == std::string(""));

    const std::string filename = "{name: !str \"testFilename\"}";
    edServer.receivePacket(createTestPacketStream(1, 3, true), {filename.begin(), filename.end()});

    REQUIRE(outputStream.str() == std::string(""));

    SECTION("The first frame is received and both are written to the output")
    {
      edServer.receivePacket(createTestPacketStream(1, 1, false), {'A', 'B'});
      REQUIRE(outputStream.str() == std::string("ABCD"));
    }
  }

  SECTION("Packets with invalid headers are not written to the output")
  {
    edServer.receivePacket(BytesBuffer(EnterpriseDiode::HeaderSizeInBytes - 1), {});

    REQUIRE(outputStream.str() == std::string(""));

    SECTION("Subsequent packet with valid header is written to the output")
    {
      edServer.receivePacket(createTestPacketStream(1, 1, false),{'X', ' '} );

      REQUIRE(outputStream.str() == std::string("X "));
    }
  }

  SECTION("Session ID is passed to the stream manager")
  {
    edServer.receivePacket(createTestPacketStream(2, 1, false), {'B', 'C'});

    REQUIRE(capturedSessionId == 2);
    REQUIRE(outputStream.str() == std::string("BC"));
  }

  SECTION("Basic Diode server does not write the CDHeader even if it implies wrapping")
  {
    edServer.receivePacket(createTestPacketStream(1, 1, false, true), {'A'});

    REQUIRE(outputStream.str() == "A");
  }
}

TEST_CASE("ED server. Import Diode")
{
  std::stringstream outputStream;
  std::uint32_t capturedSessionId = 0;
  boost::asio::io_service fake_service;
  Server edServer = createEdServer(std::make_unique<UdpServerFake>(0, fake_service, 0, 0), 16, 100,
                                   capturedSessionId, outputStream, DiodeType::import);


  SECTION("Blank CDHeader and starting bmp char is written to disk without CDHeader")
  {
    edServer.receivePacket(createTestPacketStream(1, 1, false, false), {'B'});
    REQUIRE(outputStream.str() == "B");
  }

  SECTION("Blank CDHeader and starting sisl char is written to disk without CDHeader")
  {
    edServer.receivePacket(createTestPacketStream(1, 1, false, false), {'{'});
    REQUIRE(outputStream.str() == "{");
  }

  SECTION("Blank CDHeader and starting char not sisl or bmp char is not written.")
  {
    edServer.receivePacket(createTestPacketStream(1, 1, false, false), {'A'});
    REQUIRE(outputStream.str().empty());
  }

  SECTION("Import Diode server writes the CDHeader if it implies wrapping")
  {
    edServer.receivePacket(createTestPacketStream(1, 1, false, true), {'A'});
    REQUIRE(outputStream.str() == cDHeader + "A");
  }
}

TEST_CASE("ED server. Queue length Tests")
{
  std::stringstream outputStream;
  std::uint32_t capturedSessionId = 0;
  boost::asio::io_service fake_service;

  Server edServer = createEdServer(std::make_unique<UdpServerFake>(0, fake_service, 0, 0), 16, 2,
                                   capturedSessionId, outputStream, DiodeType::basic);

  SECTION("Packets are not queued if the size of the queue is at least maxQueueLength.")
  {
    edServer.receivePacket(createTestPacketStream(1, 1, false), {'A', 'B'});
    edServer.receivePacket(createTestPacketStream(1, 3, false),{'E', 'F'});
    edServer.receivePacket(createTestPacketStream(1, 4, false), {'G', 'H'});
    edServer.receivePacket(createTestPacketStream(1, 5, false), {'I', 'J'});
    std::string filename = "{name: !str \"testFilename\"}";
    edServer.receivePacket(createTestPacketStream(1, 6, true), {filename.begin(), filename.end()});
    edServer.receivePacket(createTestPacketStream(1, 2, false), {'C', 'D'});
    REQUIRE(outputStream.str() == std::string("AB"));
  }
}

TEST_CASE("ED server. Stream is closed if timeout is exceeded.", "[integration]")
{
  std::stringstream outputStream;
  std::uint32_t capturedSessionId = 0;
  boost::asio::io_service fake_service;

  Server edServer = Server(std::make_unique<UdpServerFake>(0, fake_service, 0, 0), 16, 2,
                           [&outputStream, &capturedSessionId](std::uint32_t sessionId) {
                             capturedSessionId = sessionId;
                             return std::make_unique<StreamSpy>(outputStream, sessionId);
                           }, []() { return std::time(nullptr); }, 3, DiodeType::basic);

  edServer.receivePacket(createTestPacketStream(1, 1, false), {'A', 'B'});
  REQUIRE(outputStream.str() == std::string("AB"));
  sleep(4);

  std::string filename = "{name: !str \"testFilename\"}";
  edServer.receivePacket(createTestPacketStream(1, 2, true), {filename.begin(), filename.end()});
  REQUIRE(outputStream.str() == std::string("AB"));
}
