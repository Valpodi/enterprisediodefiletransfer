// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "Packet.hpp"
#include "diodeheader/EnterpriseDiodeHeader.hpp"

Packet parsePacket(std::vector<std::uint8_t>&& header, std::vector<std::uint8_t>&& payload)
{
  return {
    EDHeader(header).headerParams,
    std::move(payload)
  };
}