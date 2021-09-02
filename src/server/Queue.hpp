// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef ENTERPRISEDIODETESTER_QUEUE_HPP
#define ENTERPRISEDIODETESTER_QUEUE_HPP

#include "Packet.hpp"
#include <algorithm>
#include <optional>
#include <queue>
#include <mutex>

class Queue
{
public:
  explicit Queue();

  void emplace(Packet&& packet);
  void pop();
  const Packet& top();
  size_t size();
  bool empty();

private:
  std::priority_queue<Packet, std::vector<Packet>, std::greater<>> queue;
  std::mutex queueIsBusy;

};

#endif // ENTERPRISEDIODETESTER_QUEUE_HPP
