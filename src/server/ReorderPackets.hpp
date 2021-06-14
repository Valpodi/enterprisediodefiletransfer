// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <BytesBuffer.hpp>
#include <optional>
#include <queue>
#include <rewrapper/StreamingRewrapper.hpp>
#include <algorithm>
#include "Packet.hpp"

class StreamInterface;


enum class DiodeType
{
  import,
  basic
};

class ReorderPackets
{
public:
  explicit ReorderPackets(
    std::uint32_t maxBufferSize,
    std::uint32_t maxQueueLength,
    bool dropPackets,
    DiodeType diodeType,
    std::uint32_t maxFilenameLength = 65);
  bool write(Packet&& packet, StreamInterface* streamWrapper);

private:
  bool checkQueueAndWrite(StreamInterface* streamWrapper);
  void addFrameToQueue(Packet&& packet);
  std::optional<std::string> getFilenameFromStream(const BytesBuffer& eofFrame);
  std::string convertFromSisl(std::string sislFilename);


  bool queueAlreadyExceeded = false;
  std::uint32_t nextFrameCount = 1;
  std::uint32_t lastFrameReceived = 0;
  std::uint32_t maxBufferSize;
  std::uint32_t maxQueueLength;
  bool dropPackets;
  std::priority_queue<Packet, std::vector<Packet>, std::greater<>> queue;
  std::uint32_t maxFilenameLength;
  std::uint32_t maxSislLength = 1000;
  DiodeType diodeType;
  StreamingRewrapper streamingRewrapper;
  void writeFrame(StreamInterface *streamWrapper);
  void logOutOfOrderPackets(uint32_t frameCount);
};
