// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <iostream>

class StreamInterface;

class DropPackets : public PacketQueueInterface
{
public:
  bool write(std::istream&, StreamInterface*, std::uint32_t, bool eOFFlag) override
  {
    return eOFFlag;
  };
};
