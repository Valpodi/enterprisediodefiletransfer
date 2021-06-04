// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <src/test/catch.hpp>
#include "DataPackage.hpp"
#include "StreamingFileInfo.hpp"
#include "UnwrapperTestHelpers.hpp"
#include "StreamingRewrapper.hpp"
#include "CloakedDagger.hpp"

TEST_CASE("StreamingRewrapper. Wrapped files should remain wrapped")
{
  StreamingRewrapper streamingRewrapper;
  const auto rewrappedInfo = streamingRewrapper.startFile(StreamingFileInfo{"filename", 8, ResumeInfo{{0, 7, 19}, "session1"}, std::string("london")}, dummyWrappedBuffer());

  SECTION("Check file length is increased for first segment")
  {
    REQUIRE(rewrappedInfo == StreamingFileInfo{"filename", 8 + CloakedDagger::headerSize(),
          ResumeInfo{{0, 7 + CloakedDagger::headerSize(), 19 + CloakedDagger::headerSize()}, "session1"}, std::string("london")});

    SECTION("Can't call start file twice without a chunk")
    {
      REQUIRE_THROWS_AS(
        streamingRewrapper.startFile(StreamingFileInfo{"filename", 11, ResumeInfo{{8, 18, 19}, "session1"}, std::string("london")}, dummyWrappedBuffer()),
        std::logic_error);
    }

    SECTION("Check file offset is increased for subsequent segment")
    {
      streamingRewrapper.unwrap(createTestWrappedBytesBuffer("TESTDATA"));

      const auto rewrappedInfo2 = streamingRewrapper.startFile(StreamingFileInfo{"filename", 11, ResumeInfo{{8, 18, 19}, "session1"}, std::string("london")}, dummyWrappedBuffer());

      REQUIRE(rewrappedInfo2 == StreamingFileInfo{"filename", 11,
            ResumeInfo{{8 + CloakedDagger::headerSize(), 18 + CloakedDagger::headerSize(), 19 + CloakedDagger::headerSize()}, "session1"}, std::string("london")});
    }
  }

  SECTION("Throws exception if startFile called with non-zero content-range start")
  {
    REQUIRE_THROWS_AS(
      streamingRewrapper.startFile(StreamingFileInfo{"filename", 10, ResumeInfo{{1, 5, 10}, "session2"}, std::string("london")},
                                   dummyWrappedBuffer()),
      std::runtime_error);
  }

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
