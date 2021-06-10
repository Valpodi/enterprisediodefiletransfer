// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <sstream>
#include "catch.hpp"
#include "OrderingStreamWriter.hpp"
#include <EnterpriseDiodeTestHelpers.hpp>
#include "StreamSpy.hpp"

TEST_CASE("OrderingStreamWriter. Packet streams are written to the packet queue")
{
  std::stringstream outputStream;
  auto streamWriter = OrderingStreamWriter(1, 1, std::make_unique<StreamSpy>(outputStream, 1), []() { return 10000; }, false);

  std::stringstream inputStream = createTestPacketStream({'A', 'B'}, 1, 1, false);
  auto packet = parsePacket(inputStream);

  streamWriter.write(packet.payload, packet.headerParams);
  REQUIRE(outputStream.str() == "AB");
}

TEST_CASE("OrderingStreamWriter. Write returns true when the eof has been received")
{
  std::stringstream outputStream;
  auto streamWriter = OrderingStreamWriter(1, 5, std::make_unique<StreamSpy>(outputStream, 1), []() { return 10000; }, false);

  SECTION("When the EOF packet is not queued")
  {
    std::stringstream inputStream = createTestPacketStream({'A', 'B'}, 1, 1, false);
    auto packet = parsePacket(inputStream);

    std::string filename = "{name: !str \"testFilename\"}";
    std::vector<char> vcFilename(filename.begin(), filename.end());
    std::stringstream inputStream2 = createTestPacketStream(vcFilename, 1, 2, true);
    auto packet2 = parsePacket(inputStream2);

    REQUIRE_FALSE(streamWriter.write(packet.payload, packet.headerParams));
    REQUIRE(streamWriter.write(packet2.payload, packet2.headerParams));
    REQUIRE(outputStream.str() == "AB");
  }

  SECTION("When the EOF packet is queued")
  {
    std::string filename = "{name: !str \"testFilename\"}";
    std::vector<char> vcFilename(filename.begin(), filename.end());
    std::stringstream inputStreamC = createTestPacketStream(vcFilename, 1, 3, true);
    auto packetC = parsePacket(inputStreamC);

    REQUIRE_FALSE(streamWriter.write(packetC.payload, packetC.headerParams));
    REQUIRE(outputStream.str().empty());

    std::stringstream inputStreamA = createTestPacketStream({'C', 'D'}, 1, 2, false);
    auto packetA = parsePacket(inputStreamA);

    REQUIRE_FALSE(streamWriter.write(packetA.payload, packetA.headerParams));
    REQUIRE(outputStream.str().empty());

    std::stringstream inputStreamB = createTestPacketStream({'A', 'B'}, 1, 1, false);
    auto packetB = parsePacket(inputStreamB);

    REQUIRE(streamWriter.write(packetB.payload, packetB.headerParams));
    REQUIRE(outputStream.str() == "ABCD");
  }
}

TEST_CASE("OrderingStreamWriter. OrderingStreamWriter constructor sets timeLastUpdated to the the time returned by getTime")
{
  std::stringstream outputStream;
  std::uint32_t initialTime = 500;
  OrderingStreamWriter orderingStreamWriter(1,
                                            1,
                                            std::make_unique<StreamSpy>(outputStream, 1),
                                            [&initialTime]() mutable { return initialTime; }, false);

  REQUIRE(orderingStreamWriter.timeLastUpdated == 500);

  SECTION("When the stream is written to, timeLastUpdated is updated")
  {
    std::stringstream inputStream = createTestPacketStream({'A', 'B'}, 1, 1, false);
    auto packet = parsePacket(inputStream);
    initialTime = 501;

    REQUIRE(orderingStreamWriter.timeLastUpdated == 500);
    orderingStreamWriter.write(packet.payload, packet.headerParams);
    REQUIRE(orderingStreamWriter.timeLastUpdated == 501);
    REQUIRE(outputStream.str() == "AB");
  }
}