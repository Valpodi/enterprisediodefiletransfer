// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <sstream>
#include "catch.hpp"
#include "ReorderPackets.hpp"
#include "StreamSpy.hpp"

TEST_CASE("ReorderPackets. Packets received in order are written to the output")
{
  auto inputStream = std::stringstream("BC");
  std::stringstream outputStream;
  StreamSpy stream(outputStream);
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
  StreamSpy stream(outputStream);
  auto queueManager = ReorderPackets(4, 1024);
  SECTION("Handling empty filename")
  {
    auto inputStream = std::stringstream("");
    REQUIRE(queueManager.write(inputStream, &stream, 1, true));
    REQUIRE(outputStream.str().empty());
    REQUIRE(stream.storedFilename.empty());
  }
}

TEST_CASE("ReorderPackets. Out-of-order packets")
{
  std::stringstream outputStream;
  StreamSpy stream(outputStream);
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
        inputStream = std::stringstream("");
        REQUIRE(queueManager.write(inputStream, &stream, 4, true));
        REQUIRE(outputStream.str() == "ZABCDE");
      }

      SECTION("Frame 4 is EOF and a frame with frameCount above the eofFrame is ignored.")
      {
        inputStream = std::stringstream("YZ");
        REQUIRE_FALSE(queueManager.write(inputStream, &stream, 99, false));
        REQUIRE(outputStream.str() == "ZABCDE");
        inputStream = std::stringstream("");
        REQUIRE(queueManager.write(inputStream, &stream, 4, true));
        REQUIRE(outputStream.str() == "ZABCDE");
      }

      SECTION("Frame 4 is EOF and a frame with frameCount above the eofFrame and eof true is ignored.")
      {
        inputStream = std::stringstream("");
        REQUIRE_FALSE(queueManager.write(inputStream, &stream, 99, true));
        REQUIRE(outputStream.str() == "ZABCDE");
        inputStream = std::stringstream("");
        REQUIRE(queueManager.write(inputStream, &stream, 4, true));
        REQUIRE(outputStream.str() == "ZABCDE");
      }

      SECTION("Frame 5 is EOF and must be queued, "
              "a spurious eof packet with a higher frameCount does not reassign the eofFrameNumber.")
      {
        inputStream = std::stringstream("");
        REQUIRE_FALSE(queueManager.write(inputStream, &stream, 5, true));
        REQUIRE(outputStream.str() == "ZABCDE");
        inputStream = std::stringstream("");
        REQUIRE_FALSE(queueManager.write(inputStream, &stream, 99, true));
        REQUIRE(outputStream.str() == "ZABCDE");
        inputStream = std::stringstream("FG");
        REQUIRE(queueManager.write(inputStream, &stream, 4, false));
        REQUIRE(outputStream.str() == "ZABCDEFG");
      }
    }
  }

  SECTION("Packets are held in the queue until all previous packets are received")
  {
    auto inputStream = std::stringstream("BC");
    REQUIRE_FALSE(queueManager.write(inputStream, &stream, 3, false));
    REQUIRE(outputStream.str().empty());

    inputStream = std::stringstream("");
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
      auto inputStream = std::stringstream("");
      REQUIRE(queueManager.write(inputStream, &stream, 1, true));
      REQUIRE(outputStream.str() == "");
    }

    SECTION("Out-of-order EOF closes stream after all other packets have been received")
    {
      auto inputStream = std::stringstream("BC");
      REQUIRE_FALSE(queueManager.write(inputStream, &stream, 2, false));
      REQUIRE(outputStream.str().empty());

      inputStream = std::stringstream("");
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