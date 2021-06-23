// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "CloakedDagger.hpp"
#include <BytesBuffer.hpp>
#include <Parsing.hpp>
#include <array>
#include <boost/endian/conversion.hpp>

CloakedDagger::CloakedDagger(const CloakedDaggerHeader& cloakedDaggerHeader):
  magic1(Parsing::extract<std::uint32_t>(cloakedDaggerHeader, 0)),
  majorVersion(Parsing::extract<std::uint16_t>(cloakedDaggerHeader, 4)),
  minorVersion(Parsing::extract<std::uint16_t>(cloakedDaggerHeader, 6)),
  headerLength(Parsing::extract<std::uint32_t>(cloakedDaggerHeader, 8)),
  encapsulationType(Parsing::extract<std::uint32_t>(cloakedDaggerHeader, 12)),
  encapsulationConfig(Parsing::extract<std::uint16_t>(cloakedDaggerHeader, 16)),
  encapsulationDataLength(Parsing::extract<std::uint16_t>(cloakedDaggerHeader, 18)),
  key(Parsing::extract<std::array<char, maskLength>>(cloakedDaggerHeader, 20)),
  headerChecksumType(Parsing::extract<std::uint32_t>(cloakedDaggerHeader, 28)),
  headerChecksumConfig(Parsing::extract<std::uint16_t>(cloakedDaggerHeader, 32)),
  headerChecksumDataLength(Parsing::extract<std::uint16_t>(cloakedDaggerHeader, 34)),
  dataChecksumType(Parsing::extract<std::uint32_t>(cloakedDaggerHeader, 36)),
  dataChecksumDataLength(Parsing::extract<std::uint32_t>(cloakedDaggerHeader, 40)),
  magic2(Parsing::extract<std::uint32_t>(cloakedDaggerHeader, 44))
{
  throwIfHeaderInvalid();
}

void CloakedDagger::throwIfHeaderInvalid() const
{
  if ((boost::endian::native_to_big(magic1) != 0xd1df5fff) ||
      (boost::endian::native_to_big(magic2) != 0xff5fdfd1) ||
      (boost::endian::native_to_big(majorVersion) != 1) ||
      (boost::endian::native_to_big(minorVersion) != 0) ||
      (boost::endian::native_to_big(encapsulationType) != 1) ||
      (boost::endian::native_to_big(encapsulationConfig) != 3) ||
      (boost::endian::native_to_big(encapsulationDataLength) != maskLength))
  {
    throw std::runtime_error("The header did not decode correctly");
  }
}

