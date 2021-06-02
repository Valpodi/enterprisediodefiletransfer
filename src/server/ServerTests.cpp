// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <cstdint>
#include <vector>
#include <string>

#include "catch.hpp"
#include "EnterpriseDiodeTestHelpers.hpp"
#include "diodeheader/EnterpriseDiodeHeader.hpp"
#include "Server.hpp"
#include "StreamSpy.hpp"

TEST_CASE("ED server.")
{
  std::stringstream outputStream;
  std::uint32_t capturedSessionId = 0;
  boost::asio::io_service fake_service;

  Server edServer = createEdServer(std::make_unique<UdpServerFake>(0, fake_service, 0, 0), 16, 100,
                                   capturedSessionId, outputStream);


  SECTION("A packet received by the server is written to the stream")
  {
    std::stringstream stream = createTestPacketStream({'B'}, 1, 1, false);
    edServer.receivePacket(stream);

    REQUIRE(outputStream.str() == std::string("B"));

    std::string filename = "{name: !str \"testFilename\"}";
    std::vector<char> vcFilename(filename.begin(), filename.end());
    std::stringstream stream2 = createTestPacketStream(vcFilename, 1, 2, true);
    edServer.receivePacket(stream);

    REQUIRE(outputStream.str() == std::string("B"));
  }

  SECTION("Packets received are inserted into the same stream until the EOF flag is raised")
  {
    std::stringstream stream = createTestPacketStream({'B', 'C'}, 1, 1, false);
    edServer.receivePacket(stream);

    REQUIRE(outputStream.str() == std::string("BC"));

    std::stringstream stream2 = createTestPacketStream({'D'}, 1, 2, false);
    edServer.receivePacket(stream2);

    REQUIRE(outputStream.str() == std::string("BCD"));
    std::string filename = "{name: !str \"testFilename\"}";
    std::vector<char> vcFilename(filename.begin(), filename.end());
    std::stringstream stream3 = createTestPacketStream(vcFilename, 1, 3, true);
    edServer.receivePacket(stream3);

    REQUIRE(outputStream.str() == std::string("BCD"));
  }

  SECTION("Packets are reordered before being written to stream")
  {
    std::stringstream stream = createTestPacketStream({'B', 'C'}, 1, 2, false);
    edServer.receivePacket(stream);

    REQUIRE(outputStream.str() == std::string(""));

    std::string filename = "{name: !str \"testFilename\"}";
    std::vector<char> vcFilename(filename.begin(), filename.end());
    std::stringstream stream2 = createTestPacketStream(vcFilename, 1, 3, true);
    edServer.receivePacket(stream2);

    REQUIRE(outputStream.str() == std::string(""));

    SECTION("The first frame is received and both are written to the output")
    {
      std::stringstream stream3 = createTestPacketStream({'B', 'C'}, 1, 1, false);
      edServer.receivePacket(stream3);

      REQUIRE(outputStream.str() == std::string("BCBC"));
    }
  }

  SECTION("Packets with invalid headers are not written to the output")
  {
    std::vector<char> packet(EnterpriseDiode::HeaderSizeInBytes - 1);
    std::stringstream stream = std::stringstream(std::string(packet.begin(), packet.end()));
    edServer.receivePacket(stream);

    REQUIRE(outputStream.str() == std::string(""));

    SECTION("Subsequent packet with valid header is written to the output")
    {
      std::stringstream stream2 = createTestPacketStream({'X', ' '}, 1, 1, false);
      edServer.receivePacket(stream2);

      REQUIRE(outputStream.str() == std::string("X "));
    }
  }

  SECTION("Session ID is passed to the stream manager")
  {
    std::stringstream stream = createTestPacketStream({'B', 'C'}, 2, 1, false);
    edServer.receivePacket(stream);

    REQUIRE(capturedSessionId == 2);
    REQUIRE(outputStream.str() == std::string("BC"));
  }
}

TEST_CASE("ED server. Queue length Tests")
{
  std::stringstream outputStream;
  std::uint32_t capturedSessionId = 0;
  boost::asio::io_service fake_service;

  Server edServer = createEdServer(std::make_unique<UdpServerFake>(0, fake_service, 0, 0), 16, 2,
                                   capturedSessionId, outputStream);

  SECTION("Packets are not queued if the size of the queue is at least maxQueueLength.")
  {
    std::stringstream stream1 = createTestPacketStream({'A', 'B'}, 1, 1, false);
    edServer.receivePacket(stream1);

    std::stringstream stream3 = createTestPacketStream({'E', 'F'}, 1, 3, false);
    edServer.receivePacket(stream3);
    std::stringstream stream4 = createTestPacketStream({'G', 'H'}, 1, 4, false);
    edServer.receivePacket(stream4);
    std::string filename = "{name: !str \"testFilename\"}";
    std::vector<char> vcFilename(filename.begin(), filename.end());
    std::stringstream stream5 = createTestPacketStream(vcFilename, 1, 5, true);
    edServer.receivePacket(stream5);

    std::stringstream stream2 = createTestPacketStream({'C', 'D'}, 1, 2, false);
    edServer.receivePacket(stream2);
    REQUIRE(outputStream.str() == std::string("ABCDEFGH"));
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
                           }, []() { return std::time(nullptr); }, 3);

  std::stringstream stream = createTestPacketStream({'A', 'B'}, 1, 1, false);
  edServer.receivePacket(stream);
  REQUIRE(outputStream.str() == std::string("AB"));
  sleep(4);

  std::string filename = "{name: !str \"testFilename\"}";
  std::vector<char> vcFilename(filename.begin(), filename.end());
  std::stringstream stream3 = createTestPacketStream(vcFilename, 1, 2, true);
  edServer.receivePacket(stream3);
  REQUIRE(outputStream.str() == std::string("AB"));
}
