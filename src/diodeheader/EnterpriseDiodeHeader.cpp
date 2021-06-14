// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <array>
#include "EnterpriseDiodeHeader.hpp"


EDHeader::EDHeader(const std::vector<std::uint8_t>& frame) :
  headerParams(readHeaderParams(frame)),
  padding({})
{
}

template <typename T>
  T extract(const std::vector<std::uint8_t> &v, int pos)
{
  T value;
  memcpy(&value, &v[pos], sizeof(T));
  return value;
}


HeaderParams EDHeader::readHeaderParams(const std::vector<std::uint8_t>& frame)
{
  return {
    extract<std::uint32_t>(frame, 0),
    extract<std::uint32_t>(frame, 4),
    extract<bool>(frame, 8)
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