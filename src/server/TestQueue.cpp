// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "TestQueue.hpp"
#include "Packet.hpp"

TestQueue::TestQueue()
{
  //std::priority_queue<Packet, std::vector<Packet>, std::greater<>> xqueue;
  //std::mutex queueIsBusy;
}

TestQueue::TestQueue(TestQueue&& fromQueue)
{
  std::unique_lock<std::mutex> lock_a(fromQueue.queueIsBusy, std::defer_lock);
  std::unique_lock<std::mutex> lock_b(queueIsBusy, std::defer_lock);
  std::lock(lock_a, lock_b);
  std::swap(fromQueue.xqueue, xqueue);
}  

void TestQueue::emplace(Packet&& packet)
{
  std::unique_lock<std::mutex> lock_b(queueIsBusy);
  xqueue.emplace(std::move(packet));
}

const Packet& TestQueue::top()
{
  std::unique_lock<std::mutex> lock_b(queueIsBusy);
  return xqueue.top();
} 

void TestQueue::pop()
{
  std::unique_lock<std::mutex> lock_b(queueIsBusy);
  xqueue.pop();
}

size_t TestQueue::size()
{
  std::unique_lock<std::mutex> lock_b(queueIsBusy);
  return xqueue.size();
}

bool TestQueue::empty()
{
  std::unique_lock<std::mutex> lock_b(queueIsBusy);
  return xqueue.empty();
}
