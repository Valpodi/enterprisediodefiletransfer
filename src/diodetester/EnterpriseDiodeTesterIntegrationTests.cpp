// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <cstdint>
#include <vector>
#include <string>
#include <future>
#include <test/EnterpriseDiodeTestHelpers.hpp>
#include <rewrapper/UnwrapperTestHelpers.hpp>

#include "test/catch.hpp"
#include "diodeheader/EnterpriseDiodeHeader.hpp"
#include "Server.hpp"
#include "client/Client.hpp"
#include "client/Timer.hpp"
#include "client/UdpClient.hpp"
#include "UdpServer.hpp"


TEST_CASE("UDP Client. Server. Packets are only written to the output after first frame is received", "[integration]")
{
  std::stringstream outputStream;
  std::uint32_t capturedSessionId = 0;
  boost::asio::io_service io_context;
  Server edServer = createEdServer(
    std::make_unique<UdpServer>(2002, io_context, EnterpriseDiode::HeaderSizeInBytes * 2, 1024 * 1024), 1024 * 1024,
    100, capturedSessionId, outputStream, DiodeType::basic);

  std::vector<char> sendPayload{'r'};
  std::array<char, EnterpriseDiode::HeaderSizeInBytes> sendHeader{};
  sendHeader[EnterpriseDiode::FrameCountIndex] = 2;
  sendHeader[EnterpriseDiode::EOFFlagIndex] = true;

  auto clientHandle = std::async(
    std::launch::async,
    [&io_context, &sendPayload, &sendHeader]() {
      while (io_context.stopped()) { usleep(100); }
      UdpClient("localhost", 2002).send({boost::asio::buffer(sendHeader), boost::asio::buffer(sendPayload)});
    });
  auto serverHandle = std::async(std::launch::async, [&io_context]() { io_context.run(); });
  clientHandle.wait_for(std::chrono::milliseconds(1000));
  REQUIRE(outputStream.str() == std::string(""));

  std::array<char, EnterpriseDiode::HeaderSizeInBytes> sendHeaderB{};
  sendHeaderB[EnterpriseDiode::FrameCountIndex] = 1;
  sendPayload = std::vector<char>{'A', 'B', 'C'};

  auto clientHandle2 = std::async(
    std::launch::async,
    [&io_context, &sendPayload, &sendHeaderB]() {
      while (io_context.stopped()) { usleep(100); }
      UdpClient("localhost", 2002).send({boost::asio::buffer(sendHeaderB), boost::asio::buffer(sendPayload)});
    });
  std::uint32_t timeout = 2000;
  while (outputStream.str().length() < (sendPayload.size()) && (timeout-- > 0))
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  clientHandle2.wait_for(std::chrono::milliseconds(1000));
  io_context.stop();
  REQUIRE(outputStream.str() == "ABC");
}

TEST_CASE("Client. Server. Packets are sent by the client, received by the server and placed into the stream", "[integration]")
{
  std::stringstream outputStream;
  std::uint32_t capturedSessionId = 0;
  boost::asio::io_service io_context;
  Server edServer = createEdServer(
    std::make_unique<UdpServer>(2002, io_context, EnterpriseDiode::HeaderSizeInBytes * 2, 1024 * 1024), 1024 * 1024,
    100, capturedSessionId, outputStream, DiodeType::basic);

  std::stringstream sendPayload("ABCDEFGHIJKL");

  auto handle = std::async(
    std::launch::async,
    [&io_context, &sendPayload]() {
      while (io_context.stopped()) { usleep(100); }
      Client edClient( std::make_shared<UdpClient>("localhost", 2002), std::make_shared<Timer>(1000000), 1);
      edClient.send(sendPayload);
    });
  auto handle2 = std::async(std::launch::async, [&io_context]() { io_context.run(); });
  std::uint32_t timeout = 2000;
  while (outputStream.str().length() < (sendPayload.str().size()) && (timeout-- > 0))
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  io_context.stop();
  REQUIRE(outputStream.str() == sendPayload.str());
}

TEST_CASE("Udp Client. Server with import diode. Packets are rewrapped after the first packet.", "[integration]")
{
  std::stringstream outputStream;
  std::uint32_t capturedSessionId = 0;
  boost::asio::io_service io_context;
  Server edServer = createEdServer(
    std::make_unique<UdpServer>(2002, io_context, EnterpriseDiode::HeaderSizeInBytes + 5, 115), 5, 100,
    capturedSessionId, outputStream, DiodeType::import);

  std::array<char, 8> mask1 = {0x15, 0x23, 0x44, 0x45, 0x67, 0x58, 0x59, 0x6f};
  std::array<char, 8> mask2 = {0x6f, 0x12, 0x33, 0x42, 0x54, 0x52, 0x53, 0x44};
  std::array<char, 112> headerBuffer = createWrappedEDHeader(mask1);
  std::array<char, 112> headerBuffer2 = createWrappedEDHeader(mask2);
  headerBuffer2.at(EnterpriseDiode::FrameCountIndex) = 2;
  const auto sendPayload = createTestWrappedString("ABC", mask1).message;
  const auto sendPayload2 = createTestWrappedString("DEF", mask2).message;

  auto handle = std::async(
    std::launch::async,
    [&io_context, &sendPayload, &sendPayload2, &headerBuffer, &headerBuffer2]() {
      while (io_context.stopped()) { usleep(100); }
      UdpClient("localhost", 2002).send({boost::asio::buffer(headerBuffer), boost::asio::buffer(sendPayload)});
      UdpClient("localhost", 2002).send({boost::asio::buffer(headerBuffer2), boost::asio::buffer(sendPayload2)});
    });
  auto handle2 = std::async(std::launch::async, [&io_context]() { io_context.run(); });
  std::uint32_t timeout = 2000;
  while (outputStream.str().length() < (CloakedDagger::headerSize() + 6) && (timeout-- > 0))
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  std::stringstream unwrappedStream;
  unwrapFromStream(outputStream, unwrappedStream);
  REQUIRE(unwrappedStream.str() == "ABCDEF");
  io_context.stop();
}
