// Copyright PA Knowledge Ltd 2021
// For licence terms see LICENCE.md file

#include <cstdint>
#include <vector>
#include <future>

#include "catch.hpp"
#include "Client.hpp"
#include "Timer.hpp"
#include "test/EnterpriseDiodeTestHelpers.hpp"


TEST_CASE("Timer. On timer runTimer, the sendFrame is called")
{
  bool callbackWasCalled{false};
  auto timer = std::make_shared<FreeRunningTimer>();

  timer->runTimer([&callbackWasCalled]() {callbackWasCalled = true; return false;});

  REQUIRE(callbackWasCalled);
}

TEST_CASE("Timer. On manual timer tick, the sendFrame is called")
{
  bool callbackWasCalled{false};
  auto timer = std::make_shared<ManualTimer>();

  timer->runTimer([&callbackWasCalled]() { callbackWasCalled = true; return false; });

  callbackWasCalled = false;
  timer->tick();

  REQUIRE(callbackWasCalled);
}

TEST_CASE("Timer. For a timer with period 0, the sendFrame is called")
{
  bool callbackWasCalled{false};
  auto timer = std::make_shared<Timer>(0);

  timer->runTimer([&callbackWasCalled]() { callbackWasCalled = true; return false; });

  REQUIRE(callbackWasCalled);
}


TEST_CASE("Timer. Calculate the timer period for a given data rate and size packet")
{
  REQUIRE(calculateTimerPeriod(1000, 1538).total_microseconds() ==
          boost::posix_time::microseconds(12).total_microseconds());
  REQUIRE(calculateTimerPeriod(500, 1500).total_microseconds() ==
          boost::posix_time::microseconds(23).total_microseconds());
}
