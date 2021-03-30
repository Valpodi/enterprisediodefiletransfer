// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <array>
#include "catch.hpp"

#include "EnterpriseDiodeHeader.hpp"
#include "EnterpriseDiodeTestHelpers.hpp"


TEST_CASE("ED Header. Header is created from a input stream. readHeaderParams constructs HeaderParams in order.")
{
  std::array<char, EnterpriseDiode::HeaderSizeInBytes> headerBuffer{'\x03', '\x00', '\x00', '\x00',
                                                                    '\x02', '\x00', '\x00', '\x00',
                                                                    '\x01', '\x00', '\x00', '\x00',
                                                                    '\x00', '\x00', '\x00', '\x00'};
  std::stringstream ss;
  ss.write(headerBuffer.begin(), EnterpriseDiode::HeaderSizeInBytes);
  auto edHeader = EDHeader(ss);

  REQUIRE(edHeader.headerParams.sessionId == 3);
  REQUIRE(edHeader.headerParams.frameCount == 2);
  REQUIRE(edHeader.headerParams.eOFFlag == true);
}

TEST_CASE("ED Header. Header fields at maximum")
{
  std::array<char, EnterpriseDiode::HeaderSizeInBytes> headerBuffer{'\xFF', '\xFF', '\xFF', '\xFF',
                                                                    '\xFF', '\xFF', '\xFF', '\xFF',
                                                                    '\x01', '\x00', '\x00', '\x00',
                                                                    '\x00', '\x00', '\x00', '\x00'};
  std::stringstream ss;
  ss.write(headerBuffer.begin(), EnterpriseDiode::HeaderSizeInBytes);
  auto edHeader = EDHeader(ss);

  REQUIRE(edHeader.headerParams.sessionId == 0xFFFFFFFF);
  REQUIRE(edHeader.headerParams.frameCount == 0xFFFFFFFF);
  REQUIRE(edHeader.headerParams.eOFFlag == true);
}

TEST_CASE("ED Header. Header fields near maximum")
{
  std::array<char, EnterpriseDiode::HeaderSizeInBytes> headerBuffer{'\x00', '\xFF', '\xFF', '\xFF',
                                                                    '\x00', '\xFF', '\xFF', '\xFF',
                                                                    '\x01', '\x00', '\x00', '\x00',
                                                                    '\x00', '\x00', '\x00', '\x00'};
  std::stringstream ss;
  ss.write(headerBuffer.begin(), EnterpriseDiode::HeaderSizeInBytes);
  auto edHeader = EDHeader(ss);

  REQUIRE(edHeader.headerParams.sessionId == 0xFFFFFF00);
  REQUIRE(edHeader.headerParams.frameCount == 0xFFFFFF00);
  REQUIRE(edHeader.headerParams.eOFFlag == true);
}

TEST_CASE("ED Header. calculateMaxBufferSize returns max buffer size given specific MTU values.")
{
  REQUIRE(EnterpriseDiode::calculateMaxBufferSize(1500) == 1472);
  REQUIRE(EnterpriseDiode::calculateMaxBufferSize(9000) == 8972);
}
