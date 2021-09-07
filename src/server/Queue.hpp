// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef ENTERPRISEDIODETESTER_QUEUE_HPP
#define ENTERPRISEDIODETESTER_QUEUE_HPP

#include "Packet.hpp"
#include <queue>
#include <mutex>

class Queue
{
public:
  Queue();

  void emplace(Packet&& packet);
  const Packet& top();
  void pop();
  size_t size();
  bool empty();

private:
  std::priority_queue<Packet, std::vector<Packet>, std::greater<Packet>> xqueue;
  std::mutex queueIsBusy;

};

#endif // ENTERPRISEDIODETESTER_QUEUE_HPP