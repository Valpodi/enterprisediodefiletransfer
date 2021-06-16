// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef REWRAPPER_CLOAKEDDAGGERHEADER_HPP
#define REWRAPPER_CLOAKEDDAGGERHEADER_HPP

#include <cstdint>
#include <fstream>
#include <cstddef>
#include <cstring>
#include "BytesBuffer.hpp"
#include <array>

class CloakedDagger
{
public:

  explicit CloakedDagger(std::istream& inputStream);

  static constexpr size_t headerSize()   {  return 48; }
  static CloakedDagger createFromBuffer(const std::array<char, 48>& cloakedDaggerHeader);

  const static size_t maskLength {8};
  static constexpr std::uint8_t cloakedDaggerIdentifierByte {0xd1};

  std::uint32_t magic1;
  std::uint16_t majorVersion;
  std::uint16_t minorVersion;
  std::uint32_t headerLength;
  std::uint32_t encapsulationType;
  std::uint16_t encapsulationConfig;
  std::uint16_t encapsulationDataLength;
  std::array<char, maskLength> key; //encapsulationMask
  std::uint32_t headerChecksumType;
  std::uint16_t headerChecksumConfig;
  std::uint16_t headerChecksumDataLength;
  std::uint32_t dataChecksumType;
  std::uint32_t dataChecksumDataLength;
  std::uint32_t magic2;

private:
  void throwIfHeaderInvalid() const;

  template <typename T>
  static T read(std::istream& inputStream)
  {
    std::array<char, sizeof(T)> temporaryBuffer;
    inputStream.read(temporaryBuffer.data(), sizeof(T));

    checkStreamHasSpace(inputStream);

    T value;
    std::memcpy(&value, temporaryBuffer.data(), sizeof(T));

    return value;
  }

  std::array<char, maskLength> readArray(std::istream& inputStream)
  {
    std::array<char, maskLength> temporaryBuffer;
    inputStream.read(temporaryBuffer.data(), maskLength);

    checkStreamHasSpace(inputStream);

    return temporaryBuffer;
  }

  static void checkStreamHasSpace(std::istream& inputStream)
  {
    if (inputStream.rdstate() & std::ifstream::eofbit)
    {
      throw std::runtime_error("Insufficient space in file for header");
    }
  }

};


#endif //REWRAPPER_CLOAKEDDAGGERHEADER_HPP
