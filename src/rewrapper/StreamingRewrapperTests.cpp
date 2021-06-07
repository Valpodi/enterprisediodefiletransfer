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
    REQUIRE_THROWS_AS(streamingRewrapper.unwrap(input), std::runtime_error);
  }

  SECTION("For file that looks wrapped throw if header not valid")
  {
    auto input = createTestWrappedBytesBuffer("AAA");
    input[1] = 0xA;

    REQUIRE_THROWS_AS(streamingRewrapper.unwrap(input), std::runtime_error);
  }

  SECTION("For single chunk rYaml files, ensure we don't 'rewrap'")
  {
    const auto data = BytesBuffer{'{'};
    REQUIRE(streamingRewrapper.unwrap(data) == data);
  }

  SECTION("For single chunk BMP files, ensure we don't 'rewrap'")
  {
    const auto data = BytesBuffer{'B'};
    REQUIRE(streamingRewrapper.unwrap(data) == data);
  }

}
