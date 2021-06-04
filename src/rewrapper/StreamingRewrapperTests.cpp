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

  SECTION("A single chunk remains wrapped")
  {
    const auto inputWrappedFile = createTestWrappedBytesBuffer("TESTDATA");
    const auto reWrappedFile = streamingRewrapper.unwrap(inputWrappedFile);

    REQUIRE(reWrappedFile.at(0) == CloakedDagger::cloakedDaggerIdentifierByte);

    const auto doubleUnwrapped = Unwrapper().unwrap(reWrappedFile); //double unwrap to get original data back
    REQUIRE(std::string(doubleUnwrapped.begin(), doubleUnwrapped.end()) == "TESTDATA");

    REQUIRE(getMask(inputWrappedFile) == getMask(reWrappedFile));

    SECTION(
      "Two files with different session Ids are re-wrapped separately - after calling startFile, second file should be encoded with the second file's key")
    {
      streamingRewrapper.startFile(StreamingFileInfo{"filename", 8, ResumeInfo{{0, 7, 19}, "session2"}, std::string("london")}, dummyWrappedBuffer());
      const auto anotherWrappedFile = createTestWrappedBytesBuffer("NewTestData",
                                                                   {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8});
      const auto anotherReWrappedFile = streamingRewrapper.unwrap(anotherWrappedFile);
      REQUIRE(getMask(reWrappedFile) != getMask(anotherReWrappedFile));
      REQUIRE(getMask(anotherWrappedFile) == getMask(anotherReWrappedFile));

      REQUIRE(anotherReWrappedFile.at(0) == CloakedDagger::cloakedDaggerIdentifierByte);
      const auto output = Unwrapper().unwrap(anotherReWrappedFile); //double unwrap to get original data back
      REQUIRE(std::string(output.begin(), output.end()) == "NewTestData");
    }

    SECTION("Two files with the same session IDs are re-wrapped with the same cloak dagger key")
    {
      streamingRewrapper.startFile(StreamingFileInfo{"filename", 11, ResumeInfo{{8, 18, 19}, "session1"}, std::string("london")}, dummyWrappedBuffer());
      const auto anotherWrappedFile = createTestWrappedBytesBuffer("NewTestData", {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8});

      auto concatenatedFile = reWrappedFile;
      const auto secondPartOfFile = streamingRewrapper.unwrap(anotherWrappedFile);

      concatenatedFile.insert(concatenatedFile.end(), secondPartOfFile.begin(), secondPartOfFile.end());

      const auto unwrapped = Unwrapper().unwrap(concatenatedFile);

      REQUIRE(std::string(unwrapped.begin(), unwrapped.end()) == "TESTDATANewTestData");
    }

  }

  SECTION("Throws exception if startFile called with non-zero content-range start")
  {
    REQUIRE_THROWS_AS(
      streamingRewrapper.startFile(StreamingFileInfo{"filename", 10, ResumeInfo{{1, 5, 10}, "session2"}, std::string("london")},
                                   dummyWrappedBuffer()),
      std::runtime_error);
  }


  SECTION("Second chunk returns wrapped using mask from first chunk's header")
  {
    const auto wrappedOutput = streamingRewrapper.unwrap(createTestWrappedBytesBuffer("TESTDATA"));
    const auto wrappedOutput2 = streamingRewrapper.unwrap(createTestWrappedBytesBuffer("_EXTRATESTDATA"));

    auto output(wrappedOutput);
    std::copy(wrappedOutput2.begin(), wrappedOutput2.end(), std::back_inserter(output));

    const auto unwrappedOutput = Unwrapper().unwrap(output);
    REQUIRE(std::string(unwrappedOutput.begin(), unwrappedOutput.end()) == "TESTDATA_EXTRATESTDATA");
  }

  SECTION("Second chunk returns wrapped using mask from first chunk's header where first chunk not multiple of 8")
  {
    const auto wrappedOutput = streamingRewrapper.unwrap(createTestWrappedBytesBuffer("TESTDATUM"));
    const auto wrappedOutput2 = streamingRewrapper.unwrap(createTestWrappedBytesBuffer("_EXTRATESTDATA"));

    auto output(wrappedOutput);
    std::copy(wrappedOutput2.begin(), wrappedOutput2.end(), std::back_inserter(output));

    const auto unwrappedOutput = Unwrapper().unwrap(output);
    REQUIRE(std::string(unwrappedOutput.begin(), unwrappedOutput.end()) == "TESTDATUM_EXTRATESTDATA");
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
