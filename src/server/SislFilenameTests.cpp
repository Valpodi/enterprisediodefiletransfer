// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "SISLFilename.hpp"
#include <catch.hpp>

inline BytesBuffer stringToBuffer(const std::string& string)
{
  return {string.begin(), string.end()};
}

TEST_CASE("SislFilenameParsing.")
{
  SECTION("basic checks on the sisl")
  {
    SISLFilename sislFilename(50, 4);
    SECTION("Handling empty buffer") { REQUIRE(!sislFilename.extractFilename({})); }
    SECTION("Handling empty filename") { REQUIRE(!sislFilename.extractFilename(stringToBuffer("{name: !str \"\"}"))); }
    SECTION("Handling invalid sisl filename")
    {
      REQUIRE(!sislFilename.extractFilename(stringToBuffer("{name !str \"abcd\"}")));
      REQUIRE(!sislFilename.extractFilename(stringToBuffer("name: !str \"abcd\"}")));
    }
    SECTION("SISL must contain name field")
    {
      REQUIRE(!sislFilename.extractFilename(stringToBuffer("{nae: !str \"abcd\"}")));
      REQUIRE(!sislFilename.extractFilename(stringToBuffer("{}")));
    }
  }
  SECTION("sisl size is limited")
  {
    REQUIRE(!SISLFilename(18, 4).extractFilename(stringToBuffer("{name: !str \"abcd\"}")));
  }
  SECTION("simple filename")
  {
    REQUIRE(SISLFilename(19, 4).extractFilename(stringToBuffer("{name: !str \"abcd\"}")) == "abcd");
  }
  SECTION("null terminators are rejected")
  {
    REQUIRE(!SISLFilename(50, 10).extractFilename(stringToBuffer("{name: !str \"cz\\\0d\"}")));
  }
  SECTION("Filename length is limited")
  {
    REQUIRE(!SISLFilename(18, 4).extractFilename(stringToBuffer("{name: !str \"abcds\"}")));
  }
  SECTION("Filename . and - and _ characters are allowed")
  {
    REQUIRE(SISLFilename(25, 6).extractFilename(stringToBuffer("{name: !str \"a.b-c_\"}")) == "a.b-c_");
  }
  SECTION("Filename . and - and _ characters are allowed")
  {
    REQUIRE(!SISLFilename(25, 6).extractFilename(stringToBuffer("{name: !str \"abc/def\"}")));
  }
}


