// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <cstdint>
#include <vector>
#include <future>

#include "catch.hpp"

#include "diodeheader/EnterpriseDiodeHeader.hpp"
#include "UdpClient.hpp"

TEST_CASE("UDP Client. Packets are sent to the real UDP client", "[integration]")
{
  std::array<char, EnterpriseDiode::HeaderSizeInBytes> testHeader{};
  std::vector<char> testPayload('A');
  ConstSocketBuffers testPacket = {boost::asio::buffer(testHeader), boost::asio::buffer(testPayload)};
  UdpClient("localhost", 2002).send(testPacket);
}
