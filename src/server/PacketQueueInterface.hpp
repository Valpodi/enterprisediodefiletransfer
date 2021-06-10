// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <iostream>

class StreamInterface;

class PacketQueueInterface
{
public:
  virtual bool write(std::istream& inputStream, StreamInterface* streamWrapper, std::uint32_t frameCount, bool eOFFlag) = 0;
};

struct PacketQueueSettings {
  std::uint32_t maxBufferSize;
  std::uint32_t maxQueueLength;
  bool dropPackets;
};