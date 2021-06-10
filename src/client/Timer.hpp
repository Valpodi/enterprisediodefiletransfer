// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef TIMER_HPP
#define TIMER_HPP

#include <functional>
#include <boost/asio.hpp>
#include <boost/asio/high_resolution_timer.hpp>
#include "TimerInterface.hpp"

class Timer : public TimerInterface
{
public:
  explicit Timer(std::uint32_t timerPeriod);
  explicit Timer(boost::posix_time::microseconds timerPeriod);
  void runTimer(std::function<bool()> callback) override;

private:
  void tick() override;

  boost::posix_time::microseconds primaryTimerPeriod{boost::posix_time::microseconds(1000000)};
  boost::asio::io_service io;
  boost::asio::deadline_timer deadlineTimer;
};

#endif //TIMER_HPP
