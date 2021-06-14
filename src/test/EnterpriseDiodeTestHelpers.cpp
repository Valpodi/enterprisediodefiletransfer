// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <vector>
#include <sstream>
#include <StreamSpy.hpp>
#include "EnterpriseDiodeTestHelpers.hpp"
#include "diodeheader/EnterpriseDiodeHeader.hpp"

BytesBuffer createTestPacketStream(std::uint8_t sessionID, std::uint8_t frameCount, std::uint8_t eofFlag)
{
  BytesBuffer packet(EnterpriseDiode::HeaderSizeInBytes);
  if (eofFlag)
  {
    packet[EnterpriseDiode::EOFFlagIndex] = true;
  }
  packet[EnterpriseDiode::SessionIDIndex] = sessionID;
  packet[EnterpriseDiode::FrameCountIndex] = frameCount;

  return packet;
}

Server createEdServer(
  std::unique_ptr<UdpServerInterface> udpServer,
  std::uint32_t maxBufferSize,
  std::uint32_t maxQueueLength,
  std::uint32_t& capturedSessionId,
  std::stringstream& outputStream,
  DiodeType diodeType)
{
  return Server(std::move(udpServer), maxBufferSize, maxQueueLength,
                [&outputStream, &capturedSessionId](std::uint32_t sessionId) {
                  capturedSessionId = sessionId;
                  return std::make_unique<StreamSpy>(outputStream, sessionId);
                }, []() { return 10000; }, 5, diodeType);
}
