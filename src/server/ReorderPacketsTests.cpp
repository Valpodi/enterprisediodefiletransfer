// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "Packet.hpp"
#include "ReorderPackets.hpp"
#include "StreamSpy.hpp"
#include "catch.hpp"
#include <rewrapper/UnwrapperTestHelpers.hpp>

TEST_CASE("ReorderPackets. Packets received in order are written to the output")
{
  std::stringstream outputStream;
  StreamSpy stream(outputStream, 1);
  auto queueManager = ReorderPackets(4, 1024, false, DiodeType::basic);
  REQUIRE_FALSE(queueManager.write({HeaderParams{0, 1, false, {}}, {'B', 'C'}}, &stream));
  REQUIRE(outputStream.str() == "BC");

  REQUIRE_FALSE(queueManager.write({HeaderParams{0, 2, false, {}}, {'D', 'E'}}, &stream));

  REQUIRE(outputStream.str() == "BCDE");
}

TEST_CASE("ReorderPackets. Handling filename")
{
  std::stringstream outputStream;
  StreamSpy stream(outputStream, 1);
  auto queueManager = ReorderPackets(4, 1024, false, DiodeType::basic);
  SECTION("Handling empty filename")
  {
    REQUIRE(queueManager.write({HeaderParams{0, 1, true, {}}, {}}, &stream));
    REQUIRE(outputStream.str().empty());
    REQUIRE(stream.storedFilename == "rejected.12345");
  }
  SECTION("Handling empty filename value in sisl")
  {
    const auto inputStream = std::string("{name: !str \"\"}");
    REQUIRE(queueManager.write({HeaderParams{0, 1, true, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
    REQUIRE(outputStream.str().empty());
    REQUIRE(stream.storedFilename == "rejected.12345");
  }
  SECTION("Handling invalid sisl filename")
  {
    auto inputStream = std::string("name: !str \"\"}");
    REQUIRE(queueManager.write({HeaderParams{0, 1, true, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
    REQUIRE(outputStream.str().empty());
    REQUIRE(stream.storedFilename == "rejected.12345");
  }
  SECTION("Handling sisl without name key")
  {
    auto inputStream = std::string("{something: !str \"\"}");
    REQUIRE(queueManager.write({HeaderParams{0, 1, true, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
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
    auto inputStream = std::string(sislFilename);
    REQUIRE(queueManager.write({HeaderParams{0, 1, true, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
    REQUIRE(outputStream.str().empty());
    REQUIRE(stream.storedFilename == "rejected.12345");
  }
  SECTION("Handling non-empty filename")
  {
    auto inputStream = std::string("{name: !str \"testFilename\"}");
    REQUIRE(queueManager.write({HeaderParams{0, 1, true, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
    REQUIRE(outputStream.str().empty());
    REQUIRE(stream.storedFilename == "testFilename");
  }
  SECTION("Handling filename with null terminator")
  {
    auto inputStream = std::string("{name: !str \"te\0stFilename\"}");
    REQUIRE(queueManager.write({HeaderParams{0, 1, true, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
    REQUIRE(outputStream.str().empty());
    REQUIRE(stream.storedFilename == "te");
  }
  SECTION("Handling filename with length > maxFilenameLength, 65")
  {
    auto inputStream = std::string("{name: !str \"testFilenametestFilenametestFilenametestFilenametestFilenametestFilename\"}");
    REQUIRE(queueManager.write({HeaderParams{0, 1, true, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
    REQUIRE(outputStream.str().empty());
    REQUIRE(stream.storedFilename == "rejected.12345");
  }
  SECTION("Handling filename with allowable special characters")
  {
    auto inputStream = std::string("{name: !str \"example_file-name_09.txt\"}");
    REQUIRE(queueManager.write({HeaderParams{0, 1, true, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
    REQUIRE(outputStream.str().empty());
    REQUIRE(stream.storedFilename == "example_file-name_09.txt");
  }
  SECTION("Handling filename with illegal special characters")
  {
    auto inputStream = std::string("{name: !str \"/file\"}");
    REQUIRE(queueManager.write({HeaderParams{0, 1, true, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
    REQUIRE(outputStream.str().empty());
    REQUIRE(stream.storedFilename == "rejected.12345");
  }

}

TEST_CASE("ReorderPackets. Out-of-order packets")
{
  std::stringstream outputStream;
  StreamSpy stream(outputStream, 1);
  auto queueManager = ReorderPackets(16, 1024, false, DiodeType::basic);

  SECTION("Frame 2 and 3 are not written to the output if frame 1 missing")
  {
    auto inputStream = std::string("BC");
    REQUIRE_FALSE(queueManager.write({HeaderParams{0, 2, false, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
    REQUIRE(outputStream.str().empty());

    inputStream = std::string("DE");
    REQUIRE_FALSE(queueManager.write({HeaderParams{0, 3, false, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
    REQUIRE(outputStream.str().empty());

    SECTION("After frame 1 arrives, frame 2 and 3 are written to the output")
    {
      inputStream = std::string("ZA");
      REQUIRE_FALSE(queueManager.write({HeaderParams{0, 1, false, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
      REQUIRE(outputStream.str() == "ZABCDE");

      SECTION("Frame 4 is EOF and is written to to the output and write returns true")
      {
        inputStream = std::string("{name: !str \"testFilename\"}");
        REQUIRE(queueManager.write({HeaderParams{0, 4, true, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
        REQUIRE(outputStream.str() == "ZABCDE");
      }

      SECTION("Frame 5 is eof with filename, but waits for all missing packets before setting filename.")
      {
        inputStream = std::string("{name: !str \"testFilename\"}");
        REQUIRE_FALSE(queueManager.write({HeaderParams{0, 5, true, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
        REQUIRE(outputStream.str() == "ZABCDE");
        REQUIRE_FALSE(stream.storedFilename == "testFilename");
        inputStream = std::string("FG");
        REQUIRE(queueManager.write({HeaderParams{0, 4, false, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
        REQUIRE(outputStream.str() == "ZABCDEFG");
        REQUIRE(stream.storedFilename == "testFilename");
      }

      SECTION("Frame 4 is EOF and a frame with frameCount above the eofFrame is ignored.")
      {
        inputStream = std::string("YZ");
        REQUIRE_FALSE(queueManager.write({HeaderParams{0, 99, false, {}}, {inputStream.begin(), inputStream.end()}}, &stream));

        REQUIRE(outputStream.str() == "ZABCDE");
        inputStream = std::string("{name: !str \"testFilename\"}");
        REQUIRE(queueManager.write({HeaderParams{0, 4, true, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
        REQUIRE(outputStream.str() == "ZABCDE");
      }

      SECTION("Frame 4 is EOF and a frame with frameCount above the eofFrame and eof true is ignored.")
      {
        inputStream = std::string("");
        REQUIRE_FALSE(queueManager.write({HeaderParams{0, 99, true, {}}, {inputStream.begin(), inputStream.end()}}, &stream));

        REQUIRE(outputStream.str() == "ZABCDE");
        inputStream = std::string("{name: !str \"testFilename\"}");
        REQUIRE(queueManager.write({HeaderParams{0, 4, true, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
        REQUIRE(outputStream.str() == "ZABCDE");
      }

      SECTION("Frame 5 is EOF and must be queued, "
              "a spurious eof packet with a higher frameCount does not reassign the eofFrameNumber.")
      {
        inputStream = std::string("{name: !str \"testFilename\"}");
        REQUIRE_FALSE(queueManager.write({HeaderParams{0, 5, true, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
        REQUIRE(outputStream.str() == "ZABCDE");
        inputStream = std::string("{name: !str \"wrongFilename\"}");
        REQUIRE_FALSE(queueManager.write({HeaderParams{0, 99, true, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
        REQUIRE(outputStream.str() == "ZABCDE");
        inputStream = std::string("FG");
        REQUIRE(queueManager.write({HeaderParams{0, 4, false, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
        REQUIRE(outputStream.str() == "ZABCDEFG");

        REQUIRE(stream.storedFilename == "testFilename");
      }
    }
  }

  SECTION("Packets are held in the queue until all previous packets are received")
  {
    auto inputStream = std::string("BC");
    REQUIRE_FALSE(queueManager.write({HeaderParams{0, 3, false, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
    REQUIRE(outputStream.str().empty());

    inputStream = std::string("{name: !str \"testFilename\"}");
    REQUIRE_FALSE(queueManager.write({HeaderParams{0, 4, true, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
    REQUIRE(outputStream.str().empty());

    inputStream = std::string("ZA");
    REQUIRE_FALSE(queueManager.write({HeaderParams{0, 1, false, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
    REQUIRE(outputStream.str() == "ZA");

    inputStream = std::string("12");
    REQUIRE(queueManager.write({HeaderParams{0, 2, false, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
    REQUIRE(outputStream.str() == "ZA12BC");
  }

  SECTION("ReorderPackets indicates when all packets have been output to stream")
  {
    SECTION("In-order EOF closes stream immediately")
    {
      auto inputStream = std::string("{name: !str \"testFilename\"}");
      REQUIRE(queueManager.write({HeaderParams{0, 1, true, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
      REQUIRE(outputStream.str() == "");
    }

    SECTION("Out-of-order EOF closes stream after all other packets have been received")
    {
      auto inputStream = std::string("BC");
      REQUIRE_FALSE(queueManager.write({HeaderParams{0, 2, false, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
      REQUIRE(outputStream.str().empty());

      inputStream = std::string("{name: !str \"testFilename\"}");
      REQUIRE_FALSE(queueManager.write({HeaderParams{0, 4, true, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
      REQUIRE(outputStream.str().empty());

      inputStream = std::string("ZA");
      REQUIRE_FALSE(queueManager.write({HeaderParams{0, 1, false, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
      REQUIRE(outputStream.str() == "ZABC");

      inputStream = std::string("DE");
      REQUIRE(queueManager.write({HeaderParams{0, 3, false, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
      REQUIRE(outputStream.str() == "ZABCDE");
    }
  }
}

TEST_CASE("ReorderPackets. Import diode.")
{
  std::stringstream outputStream;
  StreamSpy stream(outputStream, 1);
  auto queueManager = ReorderPackets(4, 1024, false, DiodeType::import, 65);

  SECTION("Data which is not wrapped remains unchanged")
  {
    auto inputStream = std::string("BC");
    REQUIRE_FALSE(queueManager.write({HeaderParams{0, 1, false, {}}, {inputStream.begin(), inputStream.end()}}, &stream));
    REQUIRE(outputStream.str() == "BC");

    inputStream = std::string("DE");
    REQUIRE_FALSE(queueManager.write({HeaderParams{0, 2, false, {}}, {inputStream.begin(), inputStream.end()}}, &stream));

    REQUIRE(outputStream.str() == "BCDE");
  }

  SECTION("The first frame of wrapped data remains unchanged")
  {
    auto wrappedInputStream = createTestWrappedString("abc", {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0});
    REQUIRE_FALSE(queueManager.write({HeaderParams{0, 1, false, {}}, {wrappedInputStream.begin(), wrappedInputStream.end()}}, &stream));
    REQUIRE(outputStream.str() == wrappedInputStream);
  }

  SECTION("A two frame wrapped file is rewrapped with the key from the first frame.")
  {
    auto wrappedInputStream = createTestWrappedString("abc", {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0});
    queueManager.write({HeaderParams{0, 1, false, {}}, {wrappedInputStream.begin(), wrappedInputStream.end()}}, &stream);
    REQUIRE(outputStream.str() == wrappedInputStream);
    auto wrappedInputStream2 = createTestWrappedString("def", {0xf0, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0x12});
    queueManager.write({HeaderParams{0, 2, false, {}}, {wrappedInputStream2.begin(), wrappedInputStream2.end()}}, &stream);

    std::stringstream unwrappedStream;
    unwrapFromStream(outputStream, unwrappedStream);
    REQUIRE(unwrappedStream.str() == "abcdef");
  }

  SECTION("Two wrapped frames out of order are rewrapped with the key from frame with frameCount 1.")
  {
    auto wrappedInputStream = createTestWrappedString("def", {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0});
    queueManager.write({HeaderParams{0, 2, false, {}}, {wrappedInputStream.begin(), wrappedInputStream.end()}}, &stream);
    auto wrappedInputStream2 = createTestWrappedString("abc", {0xf0, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0x12});
    queueManager.write({HeaderParams{0, 1, false, {}}, {wrappedInputStream2.begin(), wrappedInputStream2.end()}}, &stream);

    std::stringstream unwrappedStream;
    unwrapFromStream(outputStream, unwrappedStream);
    REQUIRE(unwrappedStream.str() == "abcdef");
  }

  SECTION("A three frame wrapped file is rewrapped with the key from the first frame.")
  {
    auto wrappedInputStream = createTestWrappedString("abc", {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0});
    queueManager.write({HeaderParams{0, 1, false, {}}, {wrappedInputStream.begin(), wrappedInputStream.end()}}, &stream);
    auto wrappedInputStream2 = createTestWrappedString("def", {0xf0, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0x12});
    queueManager.write({HeaderParams{0, 2, false, {}}, {wrappedInputStream2.begin(), wrappedInputStream2.end()}}, &stream);
    auto wrappedInputStream3 = createTestWrappedString("ghi", {0xf5, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0x34});
    queueManager.write({HeaderParams{0, 3, false, {}}, {wrappedInputStream3.begin(), wrappedInputStream3.end()}}, &stream);
    std::string inputStream = std::string("{name: !str \"testFilename\"}");
    queueManager.write({HeaderParams{0, 4, true, {}}, {inputStream.begin(), inputStream.end()}}, &stream);

    std::stringstream unwrappedStream;
    unwrapFromStream(outputStream, unwrappedStream);
    REQUIRE(unwrappedStream.str() == "abcdefghi");
    REQUIRE(stream.storedFilename == "testFilename");
  }
}
