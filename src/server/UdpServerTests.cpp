// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <cstdint>
#include <vector>
#include <future>

#include "catch.hpp"
#include "diodeheader/EnterpriseDiodeHeader.hpp"
#include "Server.hpp"
#include "client/UdpClient.hpp"
#include "UdpServer.hpp"


TEST_CASE("UDP Server. Packets are received", "[integration]")
{
  std::vector<char> dataReceived;

  std::stringstream stuff;

  boost::asio::io_service io_context;
  UdpServer udpServer(2002, io_context, 10);
  udpServer.setCallback([&io_context, &dataReceived](BytesBuffer&&, BytesBuffer&& data) {
    std::copy(data.begin(), data.end(), std::back_inserter(dataReceived));
    io_context.stop();
  });

  std::vector<char> sendPayload(112 + 3);
  sendPayload.push_back('A');
  sendPayload.push_back('B');
  sendPayload.push_back('C');
  auto handle = std::async(
    std::launch::async,
    [&io_context, sendPayload]() {
      while (io_context.stopped()) { usleep(100); }
      UdpClient("localhost", 2002).send({boost::asio::buffer(sendPayload)});
    });

  io_context.run();
  REQUIRE(dataReceived == std::vector<char>({'A', 'B', 'C'}));
}
