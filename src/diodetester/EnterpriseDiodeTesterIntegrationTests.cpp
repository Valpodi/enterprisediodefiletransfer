// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <cstdint>
#include <vector>
#include <string>
#include <future>
#include <EnterpriseDiodeTestHelpers.hpp>

#include "catch.hpp"
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

  Server edServer = createEdServer(std::make_unique<UdpServer>(2002, io_context, 100, 1024*1024), 1024*1024, 100,
                                   capturedSessionId, outputStream);

  std::vector<char> sendPayload{'D', 'E', 'F'};
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
  REQUIRE(outputStream.str() == "ABCDEF");
}

TEST_CASE("Client. Server. Packets are sent by the client, received by the server and placed into the stream", "[integration]")
{
  std::stringstream outputStream;
  std::uint32_t capturedSessionId = 0;
  boost::asio::io_service io_context;

  Server edServer = createEdServer(std::make_unique<UdpServer>(2002, io_context, 100, 1024*1024), 1024*1024, 100,
                                   capturedSessionId, outputStream);

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
