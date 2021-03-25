// Copyright PA Knowledge Ltd 2021
// For licence terms see LICENCE.md file

#ifndef ENTERPRISEDIODETESTER_PACKET_HPP
#define ENTERPRISEDIODETESTER_PACKET_HPP


#include <cstdint>
#include <istream>

struct HeaderParams
{
  std::uint32_t sessionId;
  std::uint32_t frameCount;
  bool eOFFlag;
};


struct Packet
{
  HeaderParams headerParams;
  std::istream& payload;
};

Packet parsePacket(std::istream& payload);


#endif //ENTERPRISEDIODETESTER_PACKET_HPP
