// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <vector>
#include <sstream>
#include <StreamSpy.hpp>
#include "EnterpriseDiodeTestHelpers.hpp"
#include "diodeheader/EnterpriseDiodeHeader.hpp"

std::stringstream createTestPacketStream(std::vector<char> data, char sessionID, char frameCount, bool eofFlag)
{
  std::vector<char> packet(EnterpriseDiode::HeaderSizeInBytes);
  if (eofFlag)
  {
    packet[EnterpriseDiode::EOFFlagIndex] = true;
  }
  packet[EnterpriseDiode::SessionIDIndex] = sessionID;
  packet[EnterpriseDiode::FrameCountIndex] = frameCount;
  packet.insert(packet.end(), data.begin(), data.end());

  return std::stringstream(std::string(packet.begin(), packet.end()));
}

Server createEdServer(
  std::unique_ptr<UdpServerInterface> udpServer,
  std::uint32_t maxBufferSize,
  std::uint32_t maxQueueLength,
  std::uint32_t& capturedSessionId,
  std::stringstream& outputStream)
{
  return Server(std::move(udpServer), maxBufferSize, maxQueueLength,
                [&outputStream, &capturedSessionId](std::uint32_t sessionId) {
                  capturedSessionId = sessionId;
                  return std::make_unique<StreamSpy>(outputStream, sessionId);
                }, []() { return 10000; }, 5);
}
