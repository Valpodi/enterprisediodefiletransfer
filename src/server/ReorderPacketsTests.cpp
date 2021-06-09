// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <rewrapper/UnwrapperTestHelpers.hpp>
#include "catch.hpp"
#include "ReorderPackets.hpp"
#include "StreamSpy.hpp"

TEST_CASE("ReorderPackets. Packets received in order are written to the output")
{
  auto inputStream = std::stringstream("BC");
  std::stringstream outputStream;
  StreamSpy stream(outputStream, 1);
  auto queueManager = ReorderPackets(4, 1024);
  REQUIRE_FALSE(queueManager.write(inputStream, &stream, 1, false));
  REQUIRE(outputStream.str() == "BC");

  inputStream = std::stringstream("DE");
  REQUIRE_FALSE(queueManager.write(inputStream, &stream, 2, false));
  REQUIRE(outputStream.str() == "BCDE");
}

TEST_CASE("ReorderPackets. Handling filename")
{
  std::stringstream outputStream;
  StreamSpy stream(outputStream, 1);
  auto queueManager = ReorderPackets(4, 1024);
  SECTION("Handling empty filename")
  {
    auto inputStream = std::stringstream("");
    REQUIRE(queueManager.write(inputStream, &stream, 1, true));
    REQUIRE(outputStream.str().empty());
    REQUIRE(stream.storedFilename == "rejected.12345");
  }
  SECTION("Handling empty filename value in sisl")
  {
    auto inputStream = std::stringstream("{name: !str \"\"}");
    REQUIRE(queueManager.write(inputStream, &stream, 1, true));
    REQUIRE(outputStream.str().empty());
    REQUIRE(stream.storedFilename == "rejected.12345");
  }
  SECTION("Handling invalid sisl filename")
  {
    auto inputStream = std::stringstream("name: !str \"\"}");
    REQUIRE(queueManager.write(inputStream, &stream, 1, true));
    REQUIRE(outputStream.str().empty());
    REQUIRE(stream.storedFilename == "rejected.12345");
  }
  SECTION("Handling sisl without name key")
  {
    auto inputStream = std::stringstream("{something: !str \"\"}");
    REQUIRE(queueManager.write(inputStream, &stream, 1, true));
    REQUIRE(outputStream.str().empty());
    REQUIRE(stream.storedFilename == "rejected.12345");
  }
  SECTION("Handling sisl that exceeds max sisl size")
  {
    std::string sislFilename = "{something: !str \"";
    for(int i=0; i<1002; i++)
    {
      sislFilename += "a";
    }
    sislFilename += "\"}";
    auto inputStream = std::stringstream(sislFilename);
    REQUIRE(queueManager.write(inputStream, &stream, 1, true));
    REQUIRE(outputStream.str().empty());
    REQUIRE(stream.storedFilename == "rejected.12345");
  }
  SECTION("Handling non-empty filename")
  {
    auto inputStream = std::stringstream("{name: !str \"testFilename\"}");
    REQUIRE(queueManager.write(inputStream, &stream, 1, true));
    REQUIRE(outputStream.str().empty());
    REQUIRE(stream.storedFilename == "testFilename");
  }
  SECTION("Handling filename with null terminator")
  {
    auto inputStream = std::stringstream("{name: !str \"te\0stFilename\"}");
    REQUIRE(queueManager.write(inputStream, &stream, 1, true));
    REQUIRE(outputStream.str().empty());
    REQUIRE(stream.storedFilename == "te");
  }
  SECTION("Handling filename with length > maxFilenameLength, 65")
  {
    auto inputStream = std::stringstream("{name: !str \"testFilenametestFilenametestFilenametestFilenametestFilenametestFilename\"}");
    REQUIRE(queueManager.write(inputStream, &stream, 1, true));
    REQUIRE(outputStream.str().empty());
    REQUIRE(stream.storedFilename == "rejected.12345");
  }
  SECTION("Handling filename with allowable special characters")
  {
    auto inputStream = std::stringstream("{name: !str \"example_file-name_09.txt\"}");
    REQUIRE(queueManager.write(inputStream, &stream, 1, true));
    REQUIRE(outputStream.str().empty());
    REQUIRE(stream.storedFilename == "example_file-name_09.txt");
  }
  SECTION("Handling filename with illegal special characters")
  {
    auto inputStream = std::stringstream("{name: !str \"/file\"}");
    REQUIRE(queueManager.write(inputStream, &stream, 1, true));
    REQUIRE(outputStream.str().empty());
    REQUIRE(stream.storedFilename == "rejected.12345");
  }

}

TEST_CASE("ReorderPackets. Out-of-order packets")
{
  std::stringstream outputStream;
  StreamSpy stream(outputStream, 1);
  auto queueManager = ReorderPackets(16, 1024);

  SECTION("Frame 2 and 3 are not written to the output if frame 1 missing")
  {
    auto inputStream = std::stringstream("BC");
    REQUIRE_FALSE(queueManager.write(inputStream, &stream, 2, false));
    REQUIRE(outputStream.str().empty());

    inputStream = std::stringstream("DE");
    REQUIRE_FALSE(queueManager.write(inputStream, &stream, 3, false));
    REQUIRE(outputStream.str().empty());

    SECTION("After frame 1 arrives, frame 2 and 3 are written to the output")
    {
      inputStream = std::stringstream("ZA");
      REQUIRE_FALSE(queueManager.write(inputStream, &stream, 1, false));
      REQUIRE(outputStream.str() == "ZABCDE");

      SECTION("Frame 4 is EOF and is written to to the output and write returns true")
      {
        inputStream = std::stringstream("{name: !str \"testFilename\"}");
        REQUIRE(queueManager.write(inputStream, &stream, 4, true));
        REQUIRE(outputStream.str() == "ZABCDE");
      }

      SECTION("Frame 4 is EOF and a frame with frameCount above the eofFrame is ignored.")
      {
        inputStream = std::stringstream("YZ");
        REQUIRE_FALSE(queueManager.write(inputStream, &stream, 99, false));
        REQUIRE(outputStream.str() == "ZABCDE");
        inputStream = std::stringstream("{name: !str \"testFilename\"}");
        REQUIRE(queueManager.write(inputStream, &stream, 4, true));
        REQUIRE(outputStream.str() == "ZABCDE");
      }

      SECTION("Frame 4 is EOF and a frame with frameCount above the eofFrame and eof true is ignored.")
      {
        inputStream = std::stringstream("");
        REQUIRE_FALSE(queueManager.write(inputStream, &stream, 99, true));
        REQUIRE(outputStream.str() == "ZABCDE");
        inputStream = std::stringstream("{name: !str \"testFilename\"}");
        REQUIRE(queueManager.write(inputStream, &stream, 4, true));
        REQUIRE(outputStream.str() == "ZABCDE");
      }

      SECTION("Frame 5 is EOF and must be queued, "
              "a spurious eof packet with a higher frameCount does not reassign the eofFrameNumber.")
      {
        inputStream = std::stringstream("{name: !str \"testFilename\"}");
        REQUIRE_FALSE(queueManager.write(inputStream, &stream, 5, true));
        REQUIRE(outputStream.str() == "ZABCDE");
        inputStream = std::stringstream("{name: !str \"wrongFilename\"}");
        REQUIRE_FALSE(queueManager.write(inputStream, &stream, 99, true));
        REQUIRE(outputStream.str() == "ZABCDE");
        inputStream = std::stringstream("FG");
        REQUIRE(queueManager.write(inputStream, &stream, 4, false));
        REQUIRE(outputStream.str() == "ZABCDEFG");

        REQUIRE(stream.storedFilename == "testFilename");
      }
    }
  }

  SECTION("Packets are held in the queue until all previous packets are received")
  {
    auto inputStream = std::stringstream("BC");
    REQUIRE_FALSE(queueManager.write(inputStream, &stream, 3, false));
    REQUIRE(outputStream.str().empty());

    inputStream = std::stringstream("{name: !str \"testFilename\"}");
    REQUIRE_FALSE(queueManager.write(inputStream, &stream, 4, true));
    REQUIRE(outputStream.str().empty());

    inputStream = std::stringstream("ZA");
    REQUIRE_FALSE(queueManager.write(inputStream, &stream, 1, false));
    REQUIRE(outputStream.str() == "ZA");

    inputStream = std::stringstream("12");
    REQUIRE(queueManager.write(inputStream, &stream, 2, false));
    REQUIRE(outputStream.str() == "ZA12BC");
  }

  SECTION("ReorderPackets indicates when all packets have been output to stream")
  {
    SECTION("In-order EOF closes stream immediately")
    {
      auto inputStream = std::stringstream("{name: !str \"testFilename\"}");
      REQUIRE(queueManager.write(inputStream, &stream, 1, true));
      REQUIRE(outputStream.str() == "");
    }

    SECTION("Out-of-order EOF closes stream after all other packets have been received")
    {
      auto inputStream = std::stringstream("BC");
      REQUIRE_FALSE(queueManager.write(inputStream, &stream, 2, false));
      REQUIRE(outputStream.str().empty());

      inputStream = std::stringstream("{name: !str \"testFilename\"}");
      REQUIRE_FALSE(queueManager.write(inputStream, &stream, 4, true));
      REQUIRE(outputStream.str().empty());

      inputStream = std::stringstream("ZA");
      REQUIRE_FALSE(queueManager.write(inputStream, &stream, 1, false));
      REQUIRE(outputStream.str() == "ZABC");

      inputStream = std::stringstream("DE");
      REQUIRE(queueManager.write(inputStream, &stream, 3, false));
      REQUIRE(outputStream.str() == "ZABCDE");
    }
  }
}

TEST_CASE("ReorderPackets. Import diode.")
{
  std::stringstream outputStream;
  StreamSpy stream(outputStream, 1);
  auto queueManager = ReorderPackets(4, 1024, 65, diodeType::import);

  SECTION("Data which is not wrapped remains unchanged")
  {
    auto inputStream = std::stringstream("BC");
    REQUIRE_FALSE(queueManager.write(inputStream, &stream, 1, false));
    REQUIRE(outputStream.str() == "BC");

    inputStream = std::stringstream("DE");
    REQUIRE_FALSE(queueManager.write(inputStream, &stream, 2, false));
    REQUIRE(outputStream.str() == "BCDE");
  }

  SECTION("The first frame of wrapped data remains unchanged")
  {
    auto wrappedInputStream = createTestWrappedString("abc", {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0});
    queueManager.write(wrappedInputStream, &stream, 1, false);
    REQUIRE(outputStream.str() == wrappedInputStream.str());
  }

  SECTION("A two frame wrapped file is rewrapped with the key from the first frame.")
  {
    auto wrappedInputStream = createTestWrappedString("abcdefgh", {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0});
    queueManager.write(wrappedInputStream, &stream, 1, false);
    REQUIRE(outputStream.str() == wrappedInputStream.str());
    auto wrappedInputStream2 = createTestWrappedString("abcdefgh", {0xf0, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0x12});
    queueManager.write(wrappedInputStream2, &stream, 2, false);
    std::stringstream unwrappedStream;
    unwrapFromStream(outputStream, unwrappedStream);
    REQUIRE(unwrappedStream.str() == "abcdefghabcdefgh");
  }
}
