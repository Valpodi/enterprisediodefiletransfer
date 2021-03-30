// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "Packet.hpp"
#include "diodeheader/EnterpriseDiodeHeader.hpp"

Packet parsePacket(std::istream& payload)
{
  return {
    EDHeader(payload).headerParams,
    payload
  };
}