// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef ENTERPRISEDIODETESTER_PACKET_HPP
#define ENTERPRISEDIODETESTER_PACKET_HPP

#include "rewrapper/CloakedDagger.hpp"
#include <array>
#include <cstdint>
#include <istream>
#include <vector>

struct HeaderParams
{
  std::uint32_t sessionId;
  std::uint32_t frameCount;
  bool eOFFlag;
  std::array<char, CloakedDagger::headerSize()> cloakedDaggerHeader;
};

class Packet
{
public:
  Packet(HeaderParams&& headerParams, std::vector<std::uint8_t>&& payload):
      headerParams(std::move(headerParams)),
      payload(std::move(payload))
  {
  }

  Packet(Packet&& rhs) noexcept:
      headerParams(std::move(rhs.headerParams)),
      payload(std::move(rhs.payload)){};

  Packet& operator=(Packet&& rhs) noexcept
  {
    headerParams = std::move(rhs.headerParams);
    payload = std::move(rhs.payload);
    return *this;
  }

  Packet(const Packet&) = delete;
  Packet& operator=(Packet&) = delete;

  bool operator>(const Packet& rhs) const {
    return (headerParams.frameCount > rhs.headerParams.frameCount);
  }

  [[nodiscard]] std::vector<std::uint8_t> getFrame() const { return payload; }

  HeaderParams headerParams;
  std::vector<std::uint8_t> payload;
};

Packet parsePacket(std::vector<std::uint8_t>&& header, std::vector<std::uint8_t>&& payload);

#endif // ENTERPRISEDIODETESTER_PACKET_HPP
