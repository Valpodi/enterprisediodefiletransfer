// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <vector>
#include <sstream>
#include <StreamSpy.hpp>
#include "EnterpriseDiodeTestHelpers.hpp"
#include "diodeheader/EnterpriseDiodeHeader.hpp"

BytesBuffer createTestPacketStream(std::uint8_t sessionID, std::uint8_t frameCount, std::uint8_t eofFlag, bool wrapped)
{
  BytesBuffer packet(EnterpriseDiode::HeaderSizeInBytes);
  if (eofFlag)
  {
    packet[EnterpriseDiode::EOFFlagIndex] = true;
  }
  packet[EnterpriseDiode::SessionIDIndex] = sessionID;
  packet[EnterpriseDiode::FrameCountIndex] = frameCount;
  if (wrapped)
  {
    auto cloakedDaggerHeader = BytesBuffer{0xd1, 0xdf, 0x5f, 0xff, // magic1
                                           0x00, 0x01, // major version
                                           0x00, 0x00, // minor version
                                           0x00, 0x00, 0x00, 0x30, // total length
                                           0x00, 0x00, 0x00, 0x01, // encoding type
                                           0x00, 0x03, // encoding config
                                           0x00, 0x08, // encoding data length
                                           0x15, 0x23, 0x44, 0x45, 0x67, 0x58, 0x59, 0x6f, // mask will be here
                                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // header 1
                                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // header 2
                                           0xff, 0x5f, 0xdf, 0xd1};
    std::copy(cloakedDaggerHeader.begin(), cloakedDaggerHeader.end(), packet.begin() + 64);
  }
  return packet;
}

std::array<char, 112> createWrappedEDHeader(std::array<char, 8> mask)
{
  std::array<char, EnterpriseDiode::HeaderSizeInBytes> headerBuffer{'\x03', '\x00', '\x00', '\x00',
                                                                    '\x01', '\x00', '\x00', '\x00',
                                                                    '\x00', '\x00', '\x00', '\x00',
                                                                    '\x00', '\x00', '\x00', '\x00'};
  const auto cdHeaderBytes = BytesBuffer(
    {0xd1, 0xdf, 0x5f, 0xff, // magic1
     0x00, 0x01, // major version
     0x00, 0x00, // minor version
     0x00, 0x00, 0x00, 0x30, // total length
     0x00, 0x00, 0x00, 0x01, // encoding type
     0x00, 0x03, // encoding config
     0x00, 0x08, // encoding data length
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mask
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // header 1
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // header 2
     0xff, 0x5f, 0xdf, 0xd1});  // magic2
  std::copy(cdHeaderBytes.begin(), cdHeaderBytes.end(),
            headerBuffer.begin() + EnterpriseDiode::HeaderSizeInBytes - CloakedDagger::headerSize());
  std::copy(mask.begin(), mask.end(),
            headerBuffer.begin() + EnterpriseDiode::HeaderSizeInBytes - CloakedDagger::headerSize() + 20);
  return headerBuffer;
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
