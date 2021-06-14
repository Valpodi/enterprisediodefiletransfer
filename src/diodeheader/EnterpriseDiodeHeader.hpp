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

  explicit EDHeader(std::istream& inputStream);
  explicit EDHeader(const std::vector<std::uint8_t>& frame);

private:
  template<typename HeaderFieldType>
  static HeaderFieldType read(std::istream& inputStream)
  {
    std::array<char, sizeof(HeaderFieldType)> temporaryBuffer;
    inputStream.read(temporaryBuffer.data(), sizeof(HeaderFieldType));

    checkStreamHasSpace(inputStream);

    HeaderFieldType value;
    std::memcpy(&value, temporaryBuffer.data(), sizeof(HeaderFieldType));

    return value;
  }

  template <typename T>
  static T extract(const std::vector<std::uint8_t> &v, int pos)
  {
    T value;
    memcpy(&value, &v[pos], sizeof(T));
    return value;
  }

  static void checkStreamHasSpace(std::istream& inputStream)
  {
    if (inputStream.rdstate() & std::ifstream::eofbit)
    {
      throw std::runtime_error("Insufficient space in file for header");
    }
  }

  static void advanceByPaddingSize(std::istream& inputStream);
  static HeaderParams readHeaderParams(std::istream& inputStream);
  static HeaderParams readHeaderParams(const std::vector<std::uint8_t>& frame);


public:
  const HeaderParams headerParams;
private:
  const std::array<std::uint8_t, 7> padding;
};

#endif //EDHEADER_HPP
