// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "Timer.hpp"

Timer::Timer(std::uint32_t timerPeriod) :
  primaryTimerPeriod(boost::posix_time::microseconds(timerPeriod)),
  deadlineTimer(io) {}

Timer::Timer(boost::posix_time::microseconds timerPeriod) :
  primaryTimerPeriod(std::move(timerPeriod)),
  deadlineTimer(io) {}

void Timer::runTimer(std::function<bool()> callback)
{
  tickCallback = callback;
  deadlineTimer.expires_from_now(primaryTimerPeriod);
  tick();
  io.run();
}

void Timer::tick()
{
  deadlineTimer.expires_at(deadlineTimer.expires_at() + primaryTimerPeriod);
  if (tickCallback())
  {
    deadlineTimer.async_wait([&](const boost::system::error_code&) { tick(); });
  }
  else
  {
    io.stop();
  }
}
