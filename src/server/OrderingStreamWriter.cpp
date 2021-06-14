// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "OrderingStreamWriter.hpp"

OrderingStreamWriter::OrderingStreamWriter(
  std::uint32_t maxBufferSize,
  std::uint32_t maxQueueLength,
  bool dropPackets,
  std::unique_ptr<StreamInterface> stream,
  std::function<std::time_t()> getTime,
  DiodeType diodeType):
    packetQueue(maxBufferSize, maxQueueLength, dropPackets, diodeType),
    streamWrapper(std::move(stream)),
    getTime(std::move(getTime)),
    timeLastUpdated(this->getTime())
{
}

bool OrderingStreamWriter::write(Packet&& data)
{
  timeLastUpdated = getTime();
  return packetQueue.write(std::move(data), streamWrapper.get());
}

void OrderingStreamWriter::deleteFile()
{
  streamWrapper->deleteFile();
}

void OrderingStreamWriter::renameFile()
{
  streamWrapper->renameFile();
}
