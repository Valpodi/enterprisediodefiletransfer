// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "Queue.hpp"
#include "Packet.hpp"

Queue::Queue()
{
  std::priority_queue<Packet, std::vector<Packet>, std::greater<>> xqueue;
  std::mutex queueIsBusy;
}
  
void Queue::emplace(Packet&& packet)
{
  queueIsBusy.lock();
  xqueue.emplace(std::move(packet));
  queueIsBusy.unlock();
}

const Packet& Queue::top()
{
  queueIsBusy.lock();
  const Packet& packet = xqueue.top();
  queueIsBusy.unlock();
  return packet;
} 

void Queue::pop()
{
  queueIsBusy.lock();
  xqueue.pop();
  queueIsBusy.unlock();
}

size_t Queue::size()
{
  //queueIsBusy.lock();
  size_t size = xqueue.size();
  //queueIsBusy.unlock();
  return size;
}

bool Queue::empty()
{
  //queueIsBusy.lock();
  bool isEmpty = xqueue.empty();
  //queueIsBusy.unlock();
  return isEmpty;
}
