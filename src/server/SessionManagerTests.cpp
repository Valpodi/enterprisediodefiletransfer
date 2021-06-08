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
    auto sessionManager = SessionManager(10, 10, streamSpyCreator, fakeGetTime, 5);

    auto testPacket = createTestPacketStream({'B', 'C'}, 1, 1, false);

    REQUIRE(outputStreams.empty());
    sessionManager.writeToStream(parsePacket(testPacket));
    REQUIRE(outputStreams.at(0).str() == std::string("BC"));
    REQUIRE(capturedSessionId == 1);
    REQUIRE_FALSE(streamSpyPtr->fileDeletedWasCalled);
    REQUIRE_FALSE(streamSpyPtr->fileRenameWasCalled);

    SECTION("SessionManager renames file and closes stream given EOF packet")
    {
      std::string filename = "{name: !str \"testFilename\"}";
      std::vector<char> vcFilename(filename.begin(), filename.end());
      auto testPacket2 = createTestPacketStream(vcFilename, 1, 2, true);
      sessionManager.writeToStream(parsePacket(testPacket2));

      REQUIRE(outputStreams.at(0).str() == std::string("BC"));
      REQUIRE(streamSpyPtr->fileRenameWasCalled);

      SECTION("After a session is closed, new packets with the same sessionID are written to a new stream")
      {
        auto testPacket3 = createTestPacketStream({'F', 'G'}, 1, 1, false);
        sessionManager.writeToStream(parsePacket(testPacket3));

        REQUIRE(outputStreams.at(1).str() == std::string("FG"));
        REQUIRE_FALSE(streamSpyPtr->fileDeletedWasCalled);
        REQUIRE_FALSE(streamSpyPtr->fileRenameWasCalled);
      }
    }
  }

  SECTION("SessionManager writes to 2 streams given 2 different sessionIds.")
  {
    auto fakeGetTime = []() { return 10000; };
    auto sessionManager = SessionManager(10, 10, streamSpyCreator, fakeGetTime, 5);

    auto testPacket = createTestPacketStream({'B', 'C'}, 1, 1, false);
    sessionManager.writeToStream(parsePacket(testPacket));

    REQUIRE(outputStreams.at(0).str() == std::string("BC"));
    REQUIRE(capturedSessionId == 1);

    auto testPacket2 = createTestPacketStream({'D', 'E'}, 2, 1, false);
    sessionManager.writeToStream(parsePacket(testPacket2));

    REQUIRE(outputStreams.at(1).str() == std::string("DE"));
    REQUIRE(capturedSessionId == 2);
  }

  SECTION("SessionManager doesn't write to an expired stream, but deletes the file and closes the stream.")
  {
    std::uint32_t initialTime = 500;
    auto sessionManager = SessionManager(
      10, 10, streamSpyCreator, [&initialTime]() mutable { return initialTime; }, 15);

    auto testPacket = createTestPacketStream({'B', 'C'}, 1, 1, false);
    sessionManager.writeToStream(parsePacket(testPacket));

    REQUIRE(outputStreams.at(0).str() == std::string("BC"));
    REQUIRE(capturedSessionId == 1);
    REQUIRE_FALSE(streamSpyPtr->fileDeletedWasCalled);

    std::uint32_t secondsSinceFirstPacketSent = 20;
    initialTime += secondsSinceFirstPacketSent;

    auto testPacket2 = createTestPacketStream({'D', 'F'}, 1, 2, false);

    sessionManager.writeToStream(parsePacket(testPacket2));
    REQUIRE(outputStreams.at(0).str() == std::string("BC"));
    REQUIRE(streamSpyPtr->fileDeletedWasCalled);
    REQUIRE_FALSE(streamSpyPtr->fileRenameWasCalled);
  }

  SECTION("SessionManager doesn't rename file when queue length is exceeded.")
  {
    auto fakeGetTime = []() { return 10000; };
    auto sessionManager = SessionManager(10, 2, streamSpyCreator, fakeGetTime, 15);

    auto testPacket = createTestPacketStream({'B', 'C'}, 1, 1, false);
    sessionManager.writeToStream(parsePacket(testPacket));

    REQUIRE(outputStreams.at(0).str() == std::string("BC"));
    REQUIRE(capturedSessionId == 1);

    auto testPacket2 = createTestPacketStream({'F', 'G'}, 1, 3, false);
    sessionManager.writeToStream(parsePacket(testPacket2));

    auto testPacket3 = createTestPacketStream({'H', 'I'}, 1, 4, false);
    sessionManager.writeToStream(parsePacket(testPacket3));

    auto testPacket4 = createTestPacketStream({'J', 'K'}, 1, 5, false);
    sessionManager.writeToStream(parsePacket(testPacket4));

    auto testPacket5 = createTestPacketStream({'D', 'E'}, 1, 2, false);
    sessionManager.writeToStream(parsePacket(testPacket5));

    REQUIRE_FALSE(streamSpyPtr->fileRenameWasCalled);
  }
}
