/*
These tests use a the Catch2 header only c++ test framework: https://github.com/catchorg/Catch2
*/

#define CATCH_CONFIG_MAIN
#include "../third-party/catch.h"

#include "../numeric_cast.h"

#include <cfloat>
#include <limits>


TEST_CASE("check runtime exception has been thrown", "[std::to_integer]")
{

    using namespace std;
    SECTION("Initialise without value.")
    {
        REQUIRE_THROWS(to_integer<int8_t>(1000) == 0);
        REQUIRE_THROWS_AS( to_integer<int8_t>(1000) == 0, overflow_error);
    }

}