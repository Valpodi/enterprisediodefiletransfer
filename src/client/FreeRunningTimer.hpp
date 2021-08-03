// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file

#ifndef FREERUNNINGTIMER_HPP
#define FREERUNNINGTIMER_HPP

class FreeRunningTimer : public TimerInterface
{
public:
  void runTimer(std::function<bool()>) override;
};

#endif // FREERUNNINGTIMER_HPP
