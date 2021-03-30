// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <queue>
#include <iostream>
#include <algorithm>
#include "ReorderPackets.hpp"
#include "StreamInterface.hpp"

ReorderPackets::ReorderPackets(std::uint32_t maxBufferSize, std::uint32_t maxQueueLength) :
  maxBufferSize(maxBufferSize),
  maxQueueLength(maxQueueLength)
{
}

bool ReorderPackets::write(
  std::istream& inputStream,
  StreamInterface* streamWrapper,
  std::uint32_t frameCount,
  bool eOFFlag)
{
  if (frameCount == nextFrameCount)
  {
    streamWrapper->write(inputStream);
    ++nextFrameCount;
    if (eOFFlag)
    {
      return true;
    }
    return checkQueueAndSend(streamWrapper);
  }
  addFrameToQueue(inputStream, frameCount, eOFFlag);
  return false;
}

bool ReorderPackets::checkQueueAndSend(StreamInterface* streamWrapper)
{
  while (!queue.empty() && (queue.top().frameCount == nextFrameCount))
  {
    streamWrapper->write(queue.top().frame);
    const auto wasLastFrame = queue.top().endOfFile;
    queue.pop();
    ++nextFrameCount;
    if (wasLastFrame)
    {
      return true;
    }
  }
  return false;
}

void ReorderPackets::addFrameToQueue(std::istream& inputStream, std::uint32_t frameCount, bool endOfFile)
{
  if (queue.size() >= maxQueueLength)
  {
    std::cerr << "ReorderPackets: maxQueueLength exceeded." << std::endl;
    return;
  }

  std::vector<char> newFrame;
  newFrame.reserve(maxBufferSize);

  std::copy(
    std::istreambuf_iterator<char>(inputStream),
    std::istreambuf_iterator<char>(),
    std::back_inserter(newFrame));

  queue.emplace(FrameDetails{std::move(newFrame), frameCount, endOfFile});
}