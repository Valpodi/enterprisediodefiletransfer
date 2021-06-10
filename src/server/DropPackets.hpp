// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <chrono>
#include <iostream>

class StreamInterface;

class DropPackets : public PacketQueueInterface
{
public:
  bool write(std::istream&, StreamInterface*, std::uint32_t frameCount, bool eOFFlag) override
  {
    if (frameCount != nextFrameCount)
    {
      std::cout << std::chrono::system_clock::now().time_since_epoch().count() << " Out of order packet. Got frame no: " << frameCount << "; Number of frames out of order: " << frameCount - nextFrameCount << "\n";
      nextFrameCount = frameCount;
    }
    ++nextFrameCount;
    return eOFFlag;
  };

  std::uint32_t nextFrameCount = 1;
};
