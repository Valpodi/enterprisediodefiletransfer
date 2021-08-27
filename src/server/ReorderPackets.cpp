// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "ReorderPackets.hpp"
#include "Packet.hpp"
#include "StreamInterface.hpp"
#include "TotalFrames.hpp"
#include <chrono>
#include <iostream>
#include "spdlog/spdlog.h"
#include <boost/thread.hpp>

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
  logOutOfOrderPackets(packet.headerParams.frameCount);
  addFrameToQueue(std::move(packet));
  return checkQueueAndWrite(streamWrapper);
}

void ReorderPackets::logOutOfOrderPackets(uint32_t frameCount)
{
  if (frameCount != lastFrameReceived + 1)
  {
    //spdlog::info(std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + std::string(" Out of order frame: ") + std::to_string(frameCount));
    //spdlog::info(std::string("Last frame received was: ") + std::to_string(lastFrameReceived));
    outOfOrderFrames++;
  }
  lastFrameReceived = frameCount;
}

void ReorderPackets::addFrameToQueue(Packet&& packet)
{
  queueSize = queue.size();
  if (queueSize >= maxQueueLength)
  {
    if (!queueAlreadyExceeded)
    {
      spdlog::error("ReorderPackets: maxQueueLength exceeded.");
      spdlog::error(std::string("Last frame: ") + std::to_string(lastFrameReceived) +
                         std::string(" This frame: ") + std::to_string(packet.headerParams.frameCount));
      queueAlreadyExceeded = true;
    }
    return;
  }
  queue.emplace(std::move(packet));
  queueUsagePeak = queueSize > queueUsagePeak ? queueSize : queueUsagePeak;
}

bool ReorderPackets::checkQueueAndWrite(StreamInterface* streamWrapper)
{
  if (unloadQueueThreadState == idle)
  {
    //ReorderPackets conv_ptr;
    boost::thread queueProcessorThread(boost::bind(&ReorderPackets::unloadQueueThread, this, streamWrapper));
    unloadQueueThreadState = running;
    spdlog::info("started thread");
    return false;
  }
  else if (unloadQueueThreadState == done)
  {
    queueProcessorThread.interrupt();
    queueProcessorThread.join();
    spdlog::info("exited thread");
    return true;
  }
  else
  {
    return false;
  }
}

void ReorderPackets::unloadQueueThread(StreamInterface* streamWrapper)
{
  try
  {
    while (!queue.empty() && (queue.top().headerParams.frameCount == nextFrameCount))
    {
      if (queue.top().headerParams.eOFFlag)
      {
        streamWrapper->setStoredFilename(
          sislFilename.extractFilename(queue.top().getFrame()).value_or("rejected."));
        queue.pop();
        unloadQueueThreadState = done;
      }
      writeFrame(streamWrapper);
      queue.pop();
      ++nextFrameCount;
      boost::this_thread::sleep(boost::posix_time::microseconds(10));
    }
  }
  catch (boost::thread_interrupted&)
  {
    unloadQueueThreadState = interrupted;
    spdlog::info("exiting thread");
    return;
  }
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
