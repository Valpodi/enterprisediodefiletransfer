#Copyright PA Knowledge Ltd 2021
#MIT License. For licence terms see LICENCE.md file.

cmake_minimum_required(VERSION 3.13.4)

project(EnterpriseDiodeTester)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_STANDARD 17)


add_compile_options(-Wno-unknown-pragmas
        -Wall
        -Wextra
        -Werror
        -Wformat=2
        -Wmissing-declarations
        -Wswitch-default
        -Wunused
        -Wcast-align
        -Wfloat-equal
        -Wformat-security
        -Wconversion
        -Wpedantic)

SET(BOOSTROOT /usr/include/boost169)
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


find_package(Boost 1.69)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
message("Boost found at:" ${Boost_LIBRARY_DIRS})

include_directories(.)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/server)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/test)

add_subdirectory(src)

add_executable(UnitTests
        src/test/TestFramework.cpp
        src/diodetester/EnterpriseDiodeTesterIntegrationTests.cpp
        )

target_link_libraries(UnitTests
        ${Boost_LIBRARIES}
        -Wl,--whole-archive
        CLIENT_LIBRARY_TESTS
        SERVER_LIBRARY_TESTS
        HEADER_LIBRARY_TESTS
        -Wl,--no-whole-archive
        CLIENT_LIBRARY
        SERVER_LIBRARY
        HEADER_LIBRARY
        pthread
        stdc++fs
        )
