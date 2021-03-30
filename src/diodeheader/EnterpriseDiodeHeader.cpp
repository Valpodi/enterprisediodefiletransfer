// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <array>
#include "EnterpriseDiodeHeader.hpp"

EDHeader::EDHeader(std::istream& inputStream) :
  headerParams(readHeaderParams(inputStream)),
  padding({})
{
  advanceByPaddingSize(inputStream);
}

void EDHeader::advanceByPaddingSize(std::istream& inputStream)
{
  std::array<char, 7> temporaryBuffer{};
  inputStream.read(temporaryBuffer.data(), 7);
}

HeaderParams EDHeader::readHeaderParams(std::istream& inputStream)
{
  return {
    read<std::uint32_t>(inputStream),
    read<std::uint32_t>(inputStream),
    read<bool>(inputStream)
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