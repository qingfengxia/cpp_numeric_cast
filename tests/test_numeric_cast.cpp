/*
These tests use a the Catch2 header only c++ test framework: https://github.com/catchorg/Catch2
*/

#define CATCH_CONFIG_MAIN
#include "../third-party/catch.h"
#include "../third-party/half.hpp"

#include "../numeric_cast.h"
#if __cplusplus >= 201403L && __has_include(<boost/numeric/conversion/cast.hpp>)
#include "./test_boost_numeric_cast.cpp"
#endif

#include <cfloat>
#include <limits>


TEST_CASE("std::numeric_cast unit test", "[std::numeric_cast]")
{

    SECTION("std::numeric_cast", "builtin numeric types")
    {
        using namespace std;
        REQUIRE_THROWS(to_integer<int8_t>(1000));
        REQUIRE_THROWS_AS(to_integer<int8_t>(1000), std::overflow_error);
    }

    SECTION("Test user-defined type, half_float::half")
    {
        using namespace half_float;
        REQUIRE_THROWS_AS(std::numeric_cast<int8_t>(half{1000}), 
            std::overflow_error);
    }

}