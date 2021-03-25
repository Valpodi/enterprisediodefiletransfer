// Copyright PA Knowledge Ltd 2021
// For licence terms see LICENCE.md file

#ifndef ENTERPRISEDIODE_TIMERINTERFACE_HPP
#define ENTERPRISEDIODE_TIMERINTERFACE_HPP

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


#endif //ENTERPRISEDIODE_TIMERINTERFACE_HPP
