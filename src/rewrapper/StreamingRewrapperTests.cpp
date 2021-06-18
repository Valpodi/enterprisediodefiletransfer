// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <src/test/catch.hpp>
#include "BytesBuffer.hpp"
#include "UnwrapperTestHelpers.hpp"
#include "StreamingRewrapper.hpp"
#include "CloakedDagger.hpp"

TEST_CASE("StreamingRewrapper. Wrapped files should remain wrapped")
{
  StreamingRewrapper streamingRewrapper;
  auto header = CloakedDaggerHeader({static_cast<char>(0xd1), static_cast<char>(0xdf), 0x5f, static_cast<char>(0xff), // magic1
                                                               0x00, 0x01, // major version
                                                               0x00, 0x00, // minor version
                                                               0x00, 0x00, 0x00, 0x30, // total length
                                                               0x00, 0x00, 0x00, 0x01, // encoding type
                                                               0x00, 0x03, // encoding config
                                                               0x00, 0x08, // encoding data length
                                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mask will be here
                                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // header 1
                                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // header 2
                                                               static_cast<char>(0xff), 0x5f, static_cast<char>(0xdf), static_cast<char>(0xd1)});  // magic2


  SECTION("For file that looks wrapped throw if header not valid")
  {
    auto input = createTestWrappedString("AAA").message;
    auto invalidHeader = header;
    invalidHeader[1] = 0xA;
    REQUIRE_THROWS_AS(streamingRewrapper.rewrap(input, invalidHeader, 1), std::runtime_error);
  }

  SECTION("For single chunk rYaml files, ensure we don't 'rewrap'")
  {
    const auto data = BytesBuffer{'{'};
    REQUIRE(streamingRewrapper.rewrap(data, CloakedDaggerHeader(), 1) == data);
  }

  SECTION("Rewrap should throw if non wrapped file starts without { or B")
  {
    const auto data = BytesBuffer{'A'};
    REQUIRE_THROWS_AS(streamingRewrapper.rewrap(data, CloakedDaggerHeader(), 1), std::runtime_error);
  }

  SECTION("For single chunk BMP files, ensure we don't 'rewrap'")
  {
    const auto data = BytesBuffer{'B'};
    REQUIRE(streamingRewrapper.rewrap(data, CloakedDaggerHeader(), 1) == data);
  }

  SECTION("Rewrap is called with framecount 1, returns the input - including the header")
  {
    auto input = createTestWrappedString("AAA");
    BytesBuffer output = streamingRewrapper.rewrap(input.message, input.header, 1);
    BytesBuffer fileWritten{input.header.begin(), input.header.end()};
    fileWritten.insert(fileWritten.end(), input.message.begin(), input.message.end());
    REQUIRE(output == fileWritten);
    REQUIRE(output.at(0) == CloakedDagger::cloakedDaggerIdentifierByte);
    REQUIRE(output.size() == CloakedDagger::headerSize() + 3);
  }

  SECTION("If the first frame that rewrapper is given has frameCount != 1, throw, as the mask has not been set.")
  {
    auto input = createTestWrappedString("AAA");
    REQUIRE_THROWS_AS(streamingRewrapper.rewrap(input.message, input.header, 2), std::runtime_error);
  }

}
