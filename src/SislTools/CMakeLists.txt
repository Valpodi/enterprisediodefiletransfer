

set(SISL_TOOLS_LIBRARY_FILES
        library/RapidJsonSislConverter.cpp
        library/RapidJsonSislConverter.hpp
        library/BoostSpiritSislParser.cpp
        library/BoostSpiritSislParser.hpp
        SislTools.cpp
        SislTools.hpp)

add_library(SISL_TOOLS_TEST_LIBRARY
        ${SISL_TOOLS_LIBRARY_FILES}
        library/RapidJsonSislConverterTests.cpp
        library/BoostSpiritSislParserTests.cpp
        SislToolsTests.cpp)

add_library(SISL_TOOLS_LIBRARY
        ${SISL_TOOLS_LIBRARY_FILES})

