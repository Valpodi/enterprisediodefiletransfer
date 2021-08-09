// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "ReorderPackets.hpp"
#include "Packet.hpp"
#include "StreamInterface.hpp"
#include "TotalFrames.hpp"
#include <chrono>
#include <iostream>
#include "spdlog/spdlog.h"

ReorderPackets::ReorderPackets(
  std::uint32_t maxBufferSize,
  std::uint32_t maxQueueLength,
  DiodeType diodeType,
  std::uint32_t maxFilenameLength):
    sislFilename(maxFilenameLength),
    maxBufferSize(maxBufferSize),
    maxQueueLength(maxQueueLength),
    diodeType(diodeType)
{
}

bool ReorderPackets::write(Packet&& packet, StreamInterface* streamWrapper)
{
  ++totalFrames;
  logOutOfOrderPackets(packet.headerParams.frameCount);
  addFrameToQueue(std::move(packet));
  return checkQueueAndWrite(streamWrapper);
}

void ReorderPackets::logOutOfOrderPackets(uint32_t frameCount)
{
  if (frameCount != lastFrameReceived + 1)
  {
    spdlog::info(std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + std::string(" Out of order frame: ") + std::to_string(frameCount));
  }
  lastFrameReceived = frameCount;
}

void ReorderPackets::addFrameToQueue(Packet&& packet)
{
  if (queue.size() >= maxQueueLength)
  {
    if (!queueAlreadyExceeded)
    {
      spdlog::error("ReorderPackets: maxQueueLength exceeded.");
      queueAlreadyExceeded = true;
    }
    return;
  }
  queue.emplace(std::move(packet));
}

bool ReorderPackets::checkQueueAndWrite(StreamInterface* streamWrapper)
{
  while (!queue.empty() && (queue.top().headerParams.frameCount == nextFrameCount))
  {
    if (queue.top().headerParams.eOFFlag)
    {
      streamWrapper->setStoredFilename(
        sislFilename.extractFilename(queue.top().getFrame()).value_or("rejected."));
      queue.pop();
      return true;
    }
    writeFrame(streamWrapper);
    queue.pop();
    ++nextFrameCount;
  }
  return false;
}

void ReorderPackets::writeFrame(StreamInterface* streamWrapper)
{
  if (diodeType == DiodeType::import)
  {
    streamWrapper->write(
      streamingRewrapper.rewrap(
        queue.top().getFrame(), queue.top().headerParams.cloakedDaggerHeader,
        nextFrameCount));
  }
  else
  {
    streamWrapper->write(queue.top().getFrame());
  }
}
