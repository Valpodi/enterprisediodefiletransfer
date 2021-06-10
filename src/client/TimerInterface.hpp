// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef TIMERINTERFACE_HPP
#define TIMERINTERFACE_HPP

#include <functional>

class TimerInterface
{
public:
  virtual ~TimerInterface() = default;
  virtual void runTimer(std::function<bool()> callback) = 0;

  virtual void tick()
  {
    tickCallback();
  };

protected:
  std::function<bool()> tickCallback;
};


#endif //TIMERINTERFACE_HPP
