// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "TestQueue.hpp"
#include "Packet.hpp"

TestQueue::TestQueue()
{
  //std::priority_queue<Packet, std::vector<Packet>, std::greater<>> xqueue;
  //std::mutex queueIsBusy;
}
  
void TestQueue::emplace(Packet&& packet)
{
  queueIsBusy.lock();
  xqueue.emplace(std::move(packet));
  queueIsBusy.unlock();
}

const Packet& TestQueue::top()
{
  queueIsBusy.lock();
  const Packet& packet = xqueue.top();
  queueIsBusy.unlock();
  return packet;
} 

void TestQueue::pop()
{
  queueIsBusy.lock();
  xqueue.pop();
  queueIsBusy.unlock();
}

size_t TestQueue::size()
{
  //queueIsBusy.lock();
  size_t size = xqueue.size();
  //queueIsBusy.unlock();
  return size;
}

bool TestQueue::empty()
{
  //queueIsBusy.lock();
  bool isEmpty = xqueue.empty();
  //queueIsBusy.unlock();
  return isEmpty;
}
