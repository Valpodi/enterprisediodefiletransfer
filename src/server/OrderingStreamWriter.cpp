// Copyright PA Knowledge Ltd 2021
// For licence terms see LICENCE.md file

#include "OrderingStreamWriter.hpp"

OrderingStreamWriter::OrderingStreamWriter(
  std::uint32_t maxBufferSize,
  std::uint32_t maxQueueLength,
  std::unique_ptr<StreamInterface> streamWrapper,
  std::function<std::time_t()> getTime) :
    packetQueue(maxBufferSize, maxQueueLength),
    streamWrapper(std::move(streamWrapper)),
    getTime(std::move(getTime)),
    timeLastUpdated(this->getTime())
{
}

bool OrderingStreamWriter::write(std::istream& data, const HeaderParams& headerParams)
{
  timeLastUpdated = getTime();
  return packetQueue.write(data, streamWrapper.get(), headerParams.frameCount, headerParams.eOFFlag);
}

void OrderingStreamWriter::deleteFile()
{
  streamWrapper->deleteFile();
}

void OrderingStreamWriter::renameFile()
{
  streamWrapper->renameFile();
}
