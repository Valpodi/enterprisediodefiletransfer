// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.


#define CATCH_CONFIG_COLOUR_NONE
#define CATCH_CONFIG_FAST_COMPILE
#define CATCH_CONFIG_RUNNER
#define CATCH_CONFIG_CONSOLE_WIDTH 300

#include "catch.hpp"


int main(int argc, char *argv[])
{
    return Catch::Session().run(argc, argv);
}

