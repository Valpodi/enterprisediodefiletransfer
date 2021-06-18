
add_library (REWRAPPER_LIBRARY
        CloakedDagger.cpp
        CloakedDagger.hpp
        StreamingRewrapper.cpp
        StreamingRewrapper.hpp
        CloakedDaggerHeader.hpp)

add_library(REWRAPPER_LIBRARY_TESTS
        StreamingRewrapperTests.cpp
        UnwrapperTestHelpers.cpp
        UnwrapperTestHelpers.hpp
        )