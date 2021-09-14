// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "TestQueue.hpp"
#include "Packet.hpp"
#include "spdlog/spdlog.h"

TestQueue::TestQueue()
{
  //std::priority_queue<Packet, std::vector<Packet>, std::greater<>> queue;
  //std::mutex queueIsBusy;
}

TestQueue::TestQueue(TestQueue&& fromQueue)
{
  std::unique_lock<std::mutex> lock_a(fromQueue.queueIsBusy, std::defer_lock);
  std::unique_lock<std::mutex> lock_b(queueIsBusy, std::defer_lock);
  std::lock(lock_a, lock_b);
  std::swap(fromQueue.queue, queue);
}  

void TestQueue::emplace(Packet&& packet)
{
  std::unique_lock<std::mutex> lock_b(queueIsBusy);
  queue.emplace(std::move(packet));
}

const Packet& TestQueue::top()
{
  std::unique_lock<std::mutex> lock_b(queueIsBusy);
  return queue.top();
} 

void TestQueue::pop()
{
  std::unique_lock<std::mutex> lock_b(queueIsBusy);
  queue.pop();
}

size_t TestQueue::size()
{
  std::unique_lock<std::mutex> lock_b(queueIsBusy);
  return queue.size();
}

bool TestQueue::empty()
{
  std::unique_lock<std::mutex> lock_b(queueIsBusy);
  return queue.empty();
}

std::optional<Packet> TestQueue::nextInSequencedPacket(std::uint32_t nextFrameCount, std::uint32_t lastFrameWritten)
{
  std::unique_lock<std::mutex> lock_b(queueIsBusy);
  if (queue.empty()) return {};
  if (queue.top().headerParams.frameCount == nextFrameCount)
  {
    // Priority queue does not support moving out of top of queue, so we need const cast to remove the const reference and allow move
    Packet topFrame(std::move(const_cast<Packet&>(queue.top())));
    queue.pop();
    return {std::move(topFrame)};
  }
  if (queue.top().headerParams.frameCount <= lastFrameWritten)
  {
    spdlog::info("#discarding frame: " + std::to_string(queue.top().headerParams.frameCount));
    queue.pop();
    spdlog::info("#queue size:" + std::to_string(queue.size()));
  }
  spdlog::info("waiting for frame...");
  return {};
}
