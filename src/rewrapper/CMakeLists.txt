
add_library (UNWRAPPER_LIBRARY
        WrapperInterface.hpp
        CloakedDagger.cpp
        CloakedDagger.hpp
        DataPackage.hpp
        StreamingRewrapper.cpp
        StreamingRewrapper.hpp
        )

add_library(UNWRAPPER_LIBRARY_TESTS
        StreamingRewrapperTests.cpp
        UnwrapperTestHelpers.cpp
        UnwrapperTestHelpers.hpp
        )