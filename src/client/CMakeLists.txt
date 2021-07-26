#Copyright PA Knowledge Ltd 2021
#MIT License. For licence terms see LICENCE.md file.

add_library(CLIENT_LIBRARY
        UdpClient.cpp
        UdpClientInterface.hpp
        TimerInterface.hpp
        Timer.cpp
        Client.cpp
        ClientWrapper.cpp
        FreeRunningTimer.cpp
        FreeRunningTimer.hpp)

add_library(CLIENT_LIBRARY_TESTS
        ClientTests.cpp
        TimerTests.cpp
        UdpClientTests.cpp
        )

