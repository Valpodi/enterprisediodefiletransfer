// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef ENTERPRISEDIODETESTER_TESTQUEUE_HPP
#define ENTERPRISEDIODETESTER_TESTQUEUE_HPP

#include "Packet.hpp"
#include <queue>
#include <mutex>

class TestQueue
{
public:
  TestQueue();

  void emplace(Packet&& packet);
  const Packet& top();
  void pop();
  size_t size();
  bool empty();

private:
  std::priority_queue<Packet, std::vector<Packet>, std::greater<Packet>> xqueue;
  std::mutex queueIsBusy;

};

#endif // ENTERPRISEDIODETESTER_TESTQUEUE_HPP
