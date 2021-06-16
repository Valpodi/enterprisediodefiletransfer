// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <cstdint>
#include <vector>
#include <string>
#include <future>

#include "test/catch.hpp"

#include "test/EnterpriseDiodeTestHelpers.hpp"
#include "diodeheader/EnterpriseDiodeHeader.hpp"
#include "Client.hpp"
#include "Timer.hpp"


TEST_CASE("Client. Stream data is sent using the ED client")
{
  auto udpClientSpy = std::make_shared<UdpClientSpy>();
  Client edClient(udpClientSpy, std::make_shared<Timer>(0), 1);

  std::stringstream ss("B");
  edClient.send(ss);

  REQUIRE(udpClientSpy->buffersSent.size() == 2);
  REQUIRE(udpClientSpy->buffersSent.at(0).at(EnterpriseDiode::FrameCountIndex) == 1);
  REQUIRE(!udpClientSpy->buffersSent.at(0).at(EnterpriseDiode::EOFFlagIndex));
  REQUIRE(udpClientSpy->buffersSent.at(0).at(EnterpriseDiode::HeaderSizeInBytes) == 'B');

  REQUIRE(udpClientSpy->buffersSent.at(1).at(EnterpriseDiode::FrameCountIndex) == 2);
  REQUIRE(udpClientSpy->buffersSent.at(1).at(EnterpriseDiode::EOFFlagIndex));

  SECTION("Filename is set")
  {
    REQUIRE(udpClientSpy->buffersSent.at(1).at(EnterpriseDiode::HeaderSizeInBytes) == '{');
    REQUIRE(udpClientSpy->buffersSent.at(1).at(EnterpriseDiode::HeaderSizeInBytes + 1) == 'n');
    REQUIRE(udpClientSpy->buffersSent.at(1).at(EnterpriseDiode::HeaderSizeInBytes + 13) == 'r');
    REQUIRE(udpClientSpy->buffersSent.at(1).at(EnterpriseDiode::HeaderSizeInBytes + 20) == 'd');
  }
}

TEST_CASE("Client. Filename can be set dynamically.")
{
  auto udpClientSpy = std::make_shared<UdpClientSpy>();
  Client edClient(udpClientSpy, std::make_shared<Timer>(0), 1, "testFilename");

  std::stringstream ss("B");
  edClient.send(ss);

  REQUIRE(udpClientSpy->buffersSent.size() == 2);
  REQUIRE(udpClientSpy->buffersSent.at(0).at(EnterpriseDiode::FrameCountIndex) == 1);
  REQUIRE(!udpClientSpy->buffersSent.at(0).at(EnterpriseDiode::EOFFlagIndex));
  REQUIRE(udpClientSpy->buffersSent.at(0).at(EnterpriseDiode::HeaderSizeInBytes) == 'B');

  REQUIRE(udpClientSpy->buffersSent.at(1).at(EnterpriseDiode::FrameCountIndex) == 2);
  REQUIRE(udpClientSpy->buffersSent.at(1).at(EnterpriseDiode::EOFFlagIndex));

  SECTION("Filename is set")
  {
    REQUIRE(udpClientSpy->buffersSent.at(1).at(EnterpriseDiode::HeaderSizeInBytes) == '{');
    REQUIRE(udpClientSpy->buffersSent.at(1).at(EnterpriseDiode::HeaderSizeInBytes + 1) == 'n');
    REQUIRE(udpClientSpy->buffersSent.at(1).at(EnterpriseDiode::HeaderSizeInBytes + 13) == 't');
    REQUIRE(udpClientSpy->buffersSent.at(1).at(EnterpriseDiode::HeaderSizeInBytes + 17) == 'F');
  }
}

TEST_CASE("Client. File is sent with appropriate file name if partial file path is provided")
{
  auto udpClientSpy = std::make_shared<UdpClientSpy>();
  Client edClient(udpClientSpy, std::make_shared<Timer>(0), 1, "example/testFilename");

  std::stringstream ss("B");
  edClient.send(ss);

  REQUIRE(udpClientSpy->buffersSent.size() == 2);
  REQUIRE(udpClientSpy->buffersSent.at(0).at(EnterpriseDiode::FrameCountIndex) == 1);
  REQUIRE(!udpClientSpy->buffersSent.at(0).at(EnterpriseDiode::EOFFlagIndex));
  REQUIRE(udpClientSpy->buffersSent.at(0).at(EnterpriseDiode::HeaderSizeInBytes) == 'B');

  REQUIRE(udpClientSpy->buffersSent.at(1).at(EnterpriseDiode::FrameCountIndex) == 2);
  REQUIRE(udpClientSpy->buffersSent.at(1).at(EnterpriseDiode::EOFFlagIndex));

  SECTION("Filename is set")
  {
    REQUIRE(udpClientSpy->buffersSent.at(1).at(EnterpriseDiode::HeaderSizeInBytes) == '{');
    REQUIRE(udpClientSpy->buffersSent.at(1).at(EnterpriseDiode::HeaderSizeInBytes + 1) == 'n');
    REQUIRE(udpClientSpy->buffersSent.at(1).at(EnterpriseDiode::HeaderSizeInBytes + 13) == 't');
    REQUIRE(udpClientSpy->buffersSent.at(1).at(EnterpriseDiode::HeaderSizeInBytes + 17) == 'F');
  }
}

TEST_CASE("Client. Client throws if file is sent with an illegal file name")
{
  auto udpClientSpy = std::make_shared<UdpClientSpy>();
  Client edClient(udpClientSpy, std::make_shared<Timer>(0), 1, "testFilename!");

  std::stringstream ss("B");
  REQUIRE_THROWS_AS(edClient.send(ss), std::runtime_error);
}

TEST_CASE("Client. Client throws if file is sent with a file name that exceeds the 65 character limit.")
{
  auto udpClientSpy = std::make_shared<UdpClientSpy>();
  Client edClient(udpClientSpy, std::make_shared<Timer>(0), 1, "testFilenametestFilenametestFilenametestFilenametestFilenametestFilename");

  std::stringstream ss("B");
  REQUIRE_THROWS_AS(edClient.send(ss), std::runtime_error);
}


TEST_CASE("Client. Stream data is sent using the ED client, where maxPayloadSize is larger than data")
{
  auto udpClientSpy = std::make_shared<UdpClientSpy>();
  Client edClient(udpClientSpy, std::make_shared<Timer>(0), 10);

  std::stringstream ss("BA");
  edClient.send(ss);

  REQUIRE(udpClientSpy->buffersSent.size() == 2);
  REQUIRE(udpClientSpy->buffersSent.at(0).at(EnterpriseDiode::FrameCountIndex) == 1);
  REQUIRE(!udpClientSpy->buffersSent.at(0).at(EnterpriseDiode::EOFFlagIndex));
  REQUIRE(udpClientSpy->buffersSent.at(0).at(EnterpriseDiode::HeaderSizeInBytes) == 'B');

  REQUIRE(udpClientSpy->buffersSent.at(1).at(EnterpriseDiode::FrameCountIndex) == 2);
  REQUIRE(udpClientSpy->buffersSent.at(1).at(EnterpriseDiode::EOFFlagIndex));
}

TEST_CASE("Client. Two packets are sent using ED client when packet length is 1 and data is length 2")
{
  auto udpClientSpy = std::make_shared<UdpClientSpy>();
  Client edClient(udpClientSpy, std::make_shared<Timer>(0), 1);

  std::stringstream ss("AB");
  edClient.send(ss);

  REQUIRE(udpClientSpy->buffersSent.size() == 3);

  REQUIRE(udpClientSpy->buffersSent.at(0).at(EnterpriseDiode::FrameCountIndex) == 1);
  REQUIRE(!udpClientSpy->buffersSent.at(0).at(EnterpriseDiode::EOFFlagIndex));
  REQUIRE(udpClientSpy->buffersSent.at(0).at(EnterpriseDiode::HeaderSizeInBytes) == 'A');

  REQUIRE(udpClientSpy->buffersSent.at(1).at(EnterpriseDiode::FrameCountIndex) == 2);
  REQUIRE(!udpClientSpy->buffersSent.at(1).at(EnterpriseDiode::EOFFlagIndex));
  REQUIRE(udpClientSpy->buffersSent.at(1).at(EnterpriseDiode::HeaderSizeInBytes) == 'B');

  REQUIRE(udpClientSpy->buffersSent.at(2).at(EnterpriseDiode::FrameCountIndex) == 3);
  REQUIRE(udpClientSpy->buffersSent.at(2).at(EnterpriseDiode::EOFFlagIndex));
}

TEST_CASE("Client. Empty frame is sent when there is no source data")
{
  auto udpClientSpy = std::make_shared<UdpClientSpy>();
  Client edClient(udpClientSpy, std::make_shared<Timer>(0), 1);

  std::stringstream ss("");
  edClient.send(ss);

  REQUIRE(udpClientSpy->buffersSent.at(0).at(EnterpriseDiode::FrameCountIndex) == 1);
  REQUIRE(udpClientSpy->buffersSent.at(0).at(EnterpriseDiode::EOFFlagIndex));
}

TEST_CASE("Client. Throws exception if given a non-existent stream")
{
  auto udpClientSpy = std::make_shared<UdpClientSpy>();
  Client edClient(udpClientSpy, std::make_shared<Timer>(0), 1);

  std::fstream stream("doesNotExist");
  REQUIRE_THROWS_AS(edClient.send(stream), std::runtime_error);
}

TEST_CASE("Client. For a payload split into two packets, each packet is sent on a timer tick")
{
  auto udpClientSpy = std::make_shared<UdpClientSpy>();
  auto timerfake = std::make_shared<ManualTimer>();
  Client edClient(udpClientSpy, timerfake, 1);

  std::stringstream payload("AB");
  edClient.send(payload);


  REQUIRE(udpClientSpy->buffersSent.size() == 1);
  REQUIRE(udpClientSpy->buffersSent.at(0).at(EnterpriseDiode::FrameCountIndex) == 1);
  REQUIRE(!udpClientSpy->buffersSent.at(0).at(EnterpriseDiode::EOFFlagIndex));
  REQUIRE(udpClientSpy->buffersSent.at(0).at(EnterpriseDiode::HeaderSizeInBytes) == 'A');

  timerfake->tick();

  REQUIRE(udpClientSpy->buffersSent.size() == 2);

  REQUIRE(udpClientSpy->buffersSent.at(1).at(EnterpriseDiode::FrameCountIndex) == 2);
  REQUIRE(!udpClientSpy->buffersSent.at(1).at(EnterpriseDiode::EOFFlagIndex));
  REQUIRE(udpClientSpy->buffersSent.at(1).at(EnterpriseDiode::HeaderSizeInBytes) == 'B');

  timerfake->tick();

  REQUIRE(udpClientSpy->buffersSent.size() == 3);

  REQUIRE(udpClientSpy->buffersSent.at(2).at(EnterpriseDiode::FrameCountIndex) == 3);
  REQUIRE(udpClientSpy->buffersSent.at(2).at(EnterpriseDiode::EOFFlagIndex));
}

TEST_CASE("Client. For a payload split into two packets, each packet is sent after 1 second", "[integration]")
{
  auto udpClientSpy = std::make_shared<UdpClientSpy>();
  Client edClient(udpClientSpy, std::make_shared<Timer>(1000000), 1);

  std::stringstream payload("AB");
  edClient.send(payload);

  BytesBuffer testBytes = BytesBuffer(EnterpriseDiode::HeaderSizeInBytes + 1);
  testBytes.at(EnterpriseDiode::HeaderSizeInBytes) = 'A';
  testBytes.at(EnterpriseDiode::FrameCountIndex) = 1;

  BytesBuffer testBytesB = BytesBuffer(EnterpriseDiode::HeaderSizeInBytes + 1);
  testBytesB.at(EnterpriseDiode::HeaderSizeInBytes) = 'B';
  testBytesB.at(EnterpriseDiode::FrameCountIndex) = 2;

  BytesBuffer testBytesEOF = BytesBuffer(EnterpriseDiode::HeaderSizeInBytes + 1);
  testBytesEOF.at(EnterpriseDiode::FrameCountIndex) = 3;
  testBytesEOF.at(EnterpriseDiode::EOFFlagIndex) = true;

  REQUIRE(udpClientSpy->buffersSent.at(0).at(EnterpriseDiode::HeaderSizeInBytes) == 'A');
  REQUIRE(udpClientSpy->buffersSent.at(0).at(EnterpriseDiode::FrameCountIndex) == 1);
  REQUIRE(udpClientSpy->buffersSent.at(1).at(EnterpriseDiode::HeaderSizeInBytes) == 'B');
  REQUIRE(udpClientSpy->buffersSent.at(1).at(EnterpriseDiode::FrameCountIndex) == 2);
  REQUIRE(udpClientSpy->buffersSent.at(2).at(EnterpriseDiode::FrameCountIndex) == 3);
  REQUIRE(udpClientSpy->buffersSent.at(2).at(EnterpriseDiode::EOFFlagIndex));
  REQUIRE(udpClientSpy->buffersSent.size() == 3);
}

TEST_CASE("Client. For a multi-packet payload, with 1500B packet size, packets are sent at 100 Mbps", "[integration]")
{
  std::stringstream payload("ABABAB");
  auto udpClientSpy = std::make_shared<UdpClientSpy>();
  auto realTimer = std::make_shared<Timer>(calculateTimerPeriod(100, 1500));
  Client edClient(udpClientSpy, realTimer, 1);

  edClient.send(payload);

  BytesBuffer testBytes = BytesBuffer(EnterpriseDiode::HeaderSizeInBytes + 1);
  testBytes.at(EnterpriseDiode::HeaderSizeInBytes) = 'A';
  testBytes.at(EnterpriseDiode::FrameCountIndex) = 1;

  BytesBuffer testBytesB = BytesBuffer(EnterpriseDiode::HeaderSizeInBytes + 1);
  testBytesB.at(EnterpriseDiode::HeaderSizeInBytes) = 'B';
  testBytesB.at(EnterpriseDiode::FrameCountIndex) = 6;

  BytesBuffer testBytesEOF = BytesBuffer(EnterpriseDiode::HeaderSizeInBytes + 1);
  testBytesEOF.at(EnterpriseDiode::FrameCountIndex) = 7;
  testBytesEOF.at(EnterpriseDiode::EOFFlagIndex) = true;

  REQUIRE(udpClientSpy->buffersSent.at(0).at(EnterpriseDiode::HeaderSizeInBytes) == 'A');
  REQUIRE(udpClientSpy->buffersSent.at(0).at(EnterpriseDiode::FrameCountIndex) == 1);
  REQUIRE(udpClientSpy->buffersSent.at(5).at(EnterpriseDiode::HeaderSizeInBytes) == 'B');
  REQUIRE(udpClientSpy->buffersSent.at(5).at(EnterpriseDiode::FrameCountIndex) == 6);
  REQUIRE(udpClientSpy->buffersSent.at(6).at(EnterpriseDiode::FrameCountIndex) == 7);
  REQUIRE(udpClientSpy->buffersSent.at(6).at(EnterpriseDiode::EOFFlagIndex));
  REQUIRE(udpClientSpy->buffersSent.size() == 7);
}

TEST_CASE("Client. Packets are sent with a random session ID")
{
  auto udpClientSpy = std::make_shared<UdpClientSpy>();
  Client edClient(udpClientSpy, std::make_shared<Timer>(0), 1);

  std::stringstream ss("Hello");
  edClient.send(ss);
  auto lastSessionID = *reinterpret_cast<std::uint32_t*>(&udpClientSpy->latestPacket.at(0));

  std::stringstream nextInputStream("Diode");
  edClient.send(nextInputStream);
  REQUIRE(lastSessionID != *reinterpret_cast<std::uint32_t*>(&udpClientSpy->latestPacket.at(0)));
}
