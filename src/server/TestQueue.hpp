// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef ENTERPRISEDIODETESTER_TESTQUEUE_HPP
#define ENTERPRISEDIODETESTER_TESTQUEUE_HPP

#include "Packet.hpp"
#include <queue>
#include <mutex>
#include <optional>

class TestQueue
{
public:
  TestQueue();
  TestQueue(TestQueue&& fromQueue);
  
  void emplace(Packet&& packet);
  const Packet& top();
  void pop();
  size_t size();
  bool empty();
  std::optional<Packet> nextInSequencedPacket(std::uint32_t nextFrameCount);

private:
  std::priority_queue<Packet, std::vector<Packet>, std::greater<Packet>> queue;
  std::mutex queueIsBusy;

};

#endif // ENTERPRISEDIODETESTER_TESTQUEUE_HPP
