// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <queue>
#include <iostream>
#include <algorithm>
#include "ReorderPackets.hpp"
#include "StreamInterface.hpp"
#include <regex>

ReorderPackets::ReorderPackets(std::uint32_t maxBufferSize, std::uint32_t maxQueueLength, std::uint32_t maxFilenameLength) :
  maxBufferSize(maxBufferSize),
  maxQueueLength(maxQueueLength),
  maxFilenameLength(maxFilenameLength)
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
    if (eOFFlag)
    {
      streamWrapper->setStoredFilename(getFilenameFromStream(inputStream));
      return true;
    }
    streamWrapper->write(inputStream);
    ++nextFrameCount;
    return checkQueueAndSend(streamWrapper);
  }
  addFrameToQueue(inputStream, frameCount, eOFFlag);
  return false;
}

std::string ReorderPackets::getFilenameFromStream(std::istream& inputStream)
{
  std::string filename;
  std::copy_if(std::istreambuf_iterator<char>(inputStream), std::istreambuf_iterator<char>(), std::back_inserter(filename),
               [count = maxFilenameLength](auto&&) mutable
               { return count && count--;});
  std::regex filter("[a-zA-Z0-9\\.\\-_]+");

  return std::regex_match(filename, filter) ? filename : "rejected?filename";
}

bool ReorderPackets::checkQueueAndSend(StreamInterface* streamWrapper)
{
  while (!queue.empty() && (queue.top().frameCount == nextFrameCount))
  {
    if (queue.top().endOfFile)
    {
      queue.pop();
      return true;
    }
    streamWrapper->write(queue.top().frame);
    queue.pop();
    ++nextFrameCount;
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