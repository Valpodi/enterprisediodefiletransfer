// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "Queue.hpp"
#include "Packet.hpp"
#include <mutex>

Queue::Queue()
{
}
  
void Queue::emplace(Packet&& packet)
{
  queueIsBusy.lock();
  queue.emplace(std::move(packet));
  queueIsBusy.unlock();
}

void Queue::pop()
{
  queueIsBusy.lock();
  queue.pop();
  queueIsBusy.unlock();
}

const Packet& Queue::top()
{
  queueIsBusy.lock();
  return queue.top();
  queueIsBusy.unlock();
} 

size_t Queue::size()
{
  queueIsBusy.lock();
  return queue.size();
  queueIsBusy.unlock();
}

bool Queue::empty()
{
  queueIsBusy.lock();
  return queue.empty();
  queueIsBusy.unlock();
}
