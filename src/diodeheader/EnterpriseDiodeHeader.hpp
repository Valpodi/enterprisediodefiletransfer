// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef EDHEADER_HPP
#define EDHEADER_HPP

#include <fstream>
#include <boost/asio/buffer.hpp>
#include <Packet.hpp>

namespace EnterpriseDiode
{
  constexpr std::uint16_t HeaderSizeInBytes = 112;
  constexpr std::uint16_t ControlHeaderSizeInBytes = 16;
  constexpr std::uint16_t ControlHeaderPaddingSizeInBytes = 7;
  constexpr std::uint32_t SessionIDIndex = 0;
  constexpr std::uint32_t FrameCountIndex = 4;
  constexpr std::uint32_t EOFFlagIndex = 8;

  constexpr std::uint32_t UDPSocketSizeInBytes = 268435456;

  std::uint16_t calculateMaxBufferSize(std::uint16_t mtuSize);
}

class EDHeader
{
public:
  explicit EDHeader(const std::vector<std::uint8_t>& frame);
  HeaderParams headerParams;

private:
  template <typename T>
  static T extract(const std::vector<std::uint8_t> &v, size_t pos)
  {
    T value;
    memcpy(&value, &v[pos], sizeof(T));
    return value;
  }

  static HeaderParams readHeaderParams(const std::vector<std::uint8_t>& frame);
};

#endif //EDHEADER_HPP
