// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <sstream>
#include <EnterpriseDiodeTestHelpers.hpp>
#include "catch.hpp"
#include "SessionManager.hpp"
#include "StreamSpy.hpp"

TEST_CASE("SessionManager.")
{
  std::vector<std::stringstream> outputStreams;
  std::uint32_t capturedSessionId = 0;
  StreamSpy* streamSpyPtr;

  auto streamSpyCreator = [&outputStreams, &capturedSessionId, &streamSpyPtr](std::uint32_t sessionId) {
    capturedSessionId = sessionId;
    outputStreams.emplace_back(std::stringstream());
    auto streamSpy = std::make_unique<StreamSpy>(outputStreams.back(), capturedSessionId);
    streamSpyPtr = streamSpy.get();
    return streamSpy;
  };

  SECTION("SessionManager writes to a new stream given a new sessionId.")
  {
    auto fakeGetTime = []() { return 10000; };
    auto sessionManager = SessionManager(10, 10, streamSpyCreator, fakeGetTime, 5, DiodeType::basic);

    REQUIRE(outputStreams.empty());
    sessionManager.writeToStream(parsePacket(createTestPacketStream(1, 1, false), {'B', 'C'}));
    REQUIRE(outputStreams.at(0).str() == std::string("BC"));
    REQUIRE(capturedSessionId == 1);
    REQUIRE_FALSE(streamSpyPtr->fileDeletedWasCalled);
    REQUIRE_FALSE(streamSpyPtr->fileRenameWasCalled);

    SECTION("SessionManager renames file and closes stream given EOF packet")
    {
      const std::string filename = "{name: !str \"testFilename\"}";
      sessionManager.writeToStream(parsePacket(createTestPacketStream(1, 2, true), {filename.begin(), filename.end()}));

      REQUIRE(outputStreams.at(0).str() == std::string("BC"));
      REQUIRE(streamSpyPtr->fileRenameWasCalled);

      SECTION("After a session is closed, new packets with the same sessionID are written to a new stream")
      {
        sessionManager.writeToStream(parsePacket(createTestPacketStream(1, 1, false), {'F', 'G'}));

        REQUIRE(outputStreams.at(1).str() == std::string("FG"));
        REQUIRE_FALSE(streamSpyPtr->fileDeletedWasCalled);
        REQUIRE_FALSE(streamSpyPtr->fileRenameWasCalled);
      }
    }
  }

  SECTION("SessionManager writes to 2 streams given 2 different sessionIds.")
  {
    auto fakeGetTime = []() { return 10000; };
    auto sessionManager = SessionManager(10, 10, streamSpyCreator, fakeGetTime, 5, DiodeType::basic);

    sessionManager.writeToStream(parsePacket(createTestPacketStream(1, 1, false), {'B', 'C'}));

    REQUIRE(outputStreams.at(0).str() == std::string("BC"));
    REQUIRE(capturedSessionId == 1);

    sessionManager.writeToStream(parsePacket(createTestPacketStream(2, 1, false), {'D', 'E'}));

    REQUIRE(outputStreams.at(1).str() == std::string("DE"));
    REQUIRE(capturedSessionId == 2);
  }

  SECTION("SessionManager doesn't write to an expired stream, but deletes the file and closes the stream.")
  {
    std::uint32_t initialTime = 500;
    auto sessionManager = SessionManager(
      10, 10, streamSpyCreator, [&initialTime]() mutable { return initialTime; }, 15, DiodeType::basic);

    sessionManager.writeToStream(parsePacket(createTestPacketStream(1, 1, false), {'B', 'C'}));

    REQUIRE(outputStreams.at(0).str() == std::string("BC"));
    REQUIRE(capturedSessionId == 1);
    REQUIRE_FALSE(streamSpyPtr->fileDeletedWasCalled);

    std::uint32_t secondsSinceFirstPacketSent = 20;
    initialTime += secondsSinceFirstPacketSent;

    sessionManager.writeToStream(parsePacket(createTestPacketStream(1, 2, false), {'D', 'F'}));
    REQUIRE(outputStreams.at(0).str() == std::string("BC"));
    REQUIRE(streamSpyPtr->fileDeletedWasCalled);
    REQUIRE_FALSE(streamSpyPtr->fileRenameWasCalled);
  }

  SECTION("SessionManager doesn't rename file when queue length is exceeded.")
  {
    auto fakeGetTime = []() { return 10000; };
    auto sessionManager = SessionManager(10, 2, streamSpyCreator, fakeGetTime, 15, DiodeType::basic);

    sessionManager.writeToStream(parsePacket(createTestPacketStream(1, 1, false), {'B', 'C'}));

    REQUIRE(outputStreams.at(0).str() == std::string("BC"));
    REQUIRE(capturedSessionId == 1);

    sessionManager.writeToStream(parsePacket(createTestPacketStream(1, 3, false), {'F', 'G'}));
    sessionManager.writeToStream(parsePacket(createTestPacketStream(1, 4, false), {'H', 'I'}));
    sessionManager.writeToStream(parsePacket(createTestPacketStream(1, 5, false), {'J', 'K'}));
    sessionManager.writeToStream(parsePacket(createTestPacketStream(1, 2, false), {'D', 'E'}));

    REQUIRE_FALSE(streamSpyPtr->fileRenameWasCalled);
  }
}
