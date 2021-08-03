// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file

#include "TimerInterface.hpp"
#include "FreeRunningTimer.hpp"


void FreeRunningTimer::runTimer(std::function<bool()> callback)
{
  while (callback()) {}
}

