// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef REWRAPPER_CLOAKEDDAGGERHEADER_HPP
#define REWRAPPER_CLOAKEDDAGGERHEADER_HPP

#include <cstdint>
#include <fstream>
#include <cstddef>
#include <cstring>
#include "BytesBuffer.hpp"
#include "CloakedDaggerHeader.hpp"
#include <array>

class CloakedDagger
{
public:

  explicit CloakedDagger(const CloakedDaggerHeader& cloakedDaggerHeader);

  static constexpr size_t headerSize()   {  return 48; }
  static CloakedDagger createFromBuffer(const CloakedDaggerHeader& cloakedDaggerHeader);

  const static size_t maskLength {8};
  static constexpr std::uint8_t cloakedDaggerIdentifierByte {0xd1};

  const std::uint32_t magic1;
  const std::uint16_t majorVersion;
  const std::uint16_t minorVersion;
  const std::uint32_t headerLength;
  const std::uint32_t encapsulationType;
  const std::uint16_t encapsulationConfig;
  const std::uint16_t encapsulationDataLength;
  const std::array<char, maskLength> key; //encapsulationMask
  const std::uint32_t headerChecksumType;
  const std::uint16_t headerChecksumConfig;
  const std::uint16_t headerChecksumDataLength;
  const std::uint32_t dataChecksumType;
  const std::uint32_t dataChecksumDataLength;
  const std::uint32_t magic2;

private:
  void throwIfHeaderInvalid() const;
};


#endif //REWRAPPER_CLOAKEDDAGGERHEADER_HPP
