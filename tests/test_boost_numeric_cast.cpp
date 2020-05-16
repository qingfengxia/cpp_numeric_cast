
#include <boost/numeric/conversion/cast.hpp>

#include <iostream>
#include <climits>
#include <cfloat>   // for DBL_MAX (Peter Schmid)

#include "../third-party/catch.h"
#include "../third-party/half.hpp"


TEST_CASE("boost numeric_cast", "[boost::numeric_cast]")
{

    using namespace std;
    SECTION("Test user-defined type, half_float::half")
    {
        std::cout << "=========" << "half float" << "==============\n";
        using namespace half_float;

        int8_t v1 = boost::numeric_cast<int8_t>(half{1});
        half h1 = boost::numeric_cast<half>(100);
        std::cout << "boost::numeric_cast<half>(100) = " << h1 << std::endl;

        REQUIRE_THROWS_AS( boost::numeric_cast<int8_t>(half{1000}), boost::numeric::bad_numeric_cast);
        // equal expression 
        try{
            int8_t v1 = boost::numeric_cast<int8_t>(half{1000});
            std::cout << "boost::numeric_cast<int8_t>(half{1000}) = " << v1 << std::endl;
        }
        catch(boost::numeric::bad_numeric_cast& e)
        {
            std::cout << "`boost::numeric_cast<int8_t>(half{1000})` error : " << e.what() << '\n'; 
        }

    }


    SECTION("boost::numeric_cast for builtin types")
    {

        REQUIRE_THROWS_AS( boost::numeric_cast<int>( DBL_MAX ) == 0, boost::numeric::bad_numeric_cast);

    }

} 

