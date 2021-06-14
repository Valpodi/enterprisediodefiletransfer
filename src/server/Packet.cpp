// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "Packet.hpp"
#include "diodeheader/EnterpriseDiodeHeader.hpp"

Packet parsePacket(std::vector<std::uint8_t>&& payload)
{
  const auto headerParams = EDHeader(payload).headerParams;
  return {
    headerParams,
    std::move(payload)
  };
}