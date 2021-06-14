// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef ENTERPRISEDIODETESTER_PACKET_HPP
#define ENTERPRISEDIODETESTER_PACKET_HPP


#include <cstdint>
#include <istream>
#include <vector>

struct HeaderParams
{
  const std::uint32_t sessionId;
  const std::uint32_t frameCount;
  const bool eOFFlag;
};


struct Packet
{
  HeaderParams headerParams;
  std::vector<std::uint8_t> payload;
};

Packet parsePacket(std::vector<std::uint8_t>&& header, std::vector<std::uint8_t>&& payload);


#endif //ENTERPRISEDIODETESTER_PACKET_HPP
