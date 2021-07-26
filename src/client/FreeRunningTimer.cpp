// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file

#include "TimerInterface.hpp"

class FreeRunningTimer : public TimerInterface
{
public:
  void runTimer(std::function<bool()> callback) override
  {
    while (callback()) {}
  }
};
