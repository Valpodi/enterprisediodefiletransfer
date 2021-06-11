// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "OrderingStreamWriter.hpp"
#include "DropPackets.hpp"

OrderingStreamWriter::OrderingStreamWriter(
  std::uint32_t maxBufferSize,
  std::uint32_t maxQueueLength,
  bool dropPackets,
  std::unique_ptr<StreamInterface> stream,
  std::function<std::time_t()> getTime,
  DiodeType diodeType) :
    streamWrapper(std::move(stream)),
    getTime(std::move(getTime)),
    timeLastUpdated(this->getTime())
{
  if (!dropPackets)
  {
    packetQueue = std::make_unique<ReorderPackets>(maxBufferSize, maxQueueLength, diodeType);
  }
  else
  {
    packetQueue = std::make_unique<DropPackets>();
  }
}

bool OrderingStreamWriter::write(std::istream& data, const HeaderParams& headerParams)
{
  timeLastUpdated = getTime();
  return packetQueue->write(data, streamWrapper.get(), headerParams.frameCount, headerParams.eOFFlag);
}

void OrderingStreamWriter::deleteFile()
{
  streamWrapper->deleteFile();
}

void OrderingStreamWriter::renameFile()
{
  streamWrapper->renameFile();
}
