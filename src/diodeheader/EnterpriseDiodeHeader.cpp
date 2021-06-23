// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "EnterpriseDiodeHeader.hpp"
#include <Parsing.hpp>

EDHeader::EDHeader(const std::vector<std::uint8_t>& frame) :
  headerParams(readHeaderParams(frame))
{
}


HeaderParams EDHeader::readHeaderParams(const std::vector<std::uint8_t>& frame)
{
  return {
    Parsing::extract<std::uint32_t>(frame, 0),
    Parsing::extract<std::uint32_t>(frame, 4),
    Parsing::extract<bool>(frame, 8),
    Parsing::extract_array(frame, EnterpriseDiode::HeaderSizeInBytes - CloakedDagger::headerSize())
  };
}

namespace EnterpriseDiode
{
  std::uint16_t calculateMaxBufferSize(std::uint16_t mtuSize)
  {
    // IP frame size = 20 & UDP frame size = 8
    return std::uint16_t(((short) mtuSize - 20) - 8);
  }
}