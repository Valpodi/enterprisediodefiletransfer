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


  SECTION("For file that looks wrapped throw if not enough data for header")
  {
    const BytesBuffer input{0xd1};
    REQUIRE_THROWS_AS(streamingRewrapper.rewrap(input, 1), std::runtime_error);
  }

  SECTION("For file that looks wrapped throw if header not valid")
  {
    auto input = createTestWrappedBytesBuffer("AAA");
    input[1] = 0xA;

    REQUIRE_THROWS_AS(streamingRewrapper.rewrap(input, 1), std::runtime_error);
  }

  SECTION("For single chunk rYaml files, ensure we don't 'rewrap'")
  {
    const auto data = BytesBuffer{'{'};
    REQUIRE(streamingRewrapper.rewrap(data, 1) == data);
  }

  SECTION("For single chunk BMP files, ensure we don't 'rewrap'")
  {
    const auto data = BytesBuffer{'B'};
    REQUIRE(streamingRewrapper.rewrap(data, 1) == data);
  }

  SECTION("Rewrap is called with framecount 1, returns the input - including the header")
  {
    auto input = createTestWrappedBytesBuffer("AAA");
    auto output = streamingRewrapper.rewrap(input, 1);
    REQUIRE(output == input);
    REQUIRE(output.at(0) == CloakedDagger::cloakedDaggerIdentifierByte);
    REQUIRE(output.size() == CloakedDagger::headerSize() + 3);
  }

  SECTION("If the first frame that rewrapper is given has frameCount != 1, the mask will not be set.")
  {
    auto input = createTestWrappedBytesBuffer("AAA");
    REQUIRE_THROWS_AS(streamingRewrapper.rewrap(input, 2), std::runtime_error);
  }

}
