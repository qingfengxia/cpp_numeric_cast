
#ifdef USE_BOOST_SAFE_NUMERICS
#include <boost/numeric/conversion/cast.hpp>
#include <boost/safe_numerics/safe_integer.hpp>
#else
#error "boost safe numerics lib has not been installed and enabled"
#endif

#include "demo.h"

void test()
{
    std::cout << "================ boost safe numerics =============" << std::endl;
    using namespace boost::safe_numerics;
    static_assert(std::is_numeric<safe<std::int32_t>>::value, "");
    try{
        
        safe<std::int32_t> x = INT_MAX;
        safe<std::int32_t> y = 100;
        safe<std::int32_t> z;
        // rather than producing an invalid result an exception is thrown
        z = x + y;
    }
    catch(std::exception & e){
        // which we can catch here
        std::cout << "error detected:" << e.what() << std::endl;
    }
}

int main()
{
    using namespace boost::safe_numerics;
    std::cout << std::boolalpha;
    
    test();

    //test_conversion<double, safe<std::int32_t>>("to_integer");
/*
    test_conversion<double, unsigned int>("to_unsigned");
    test_conversion<double, uint64_t>("to_integer");
    test_conversion<double, int64_t>("to_numeric");
    test_conversion<int64_t, bool>("to_integer");
    //test_conversion<int64_t, std::byte>("to_unsigned");
    test_conversion<int32_t, char16_t>("to_integer");

    test_conversion<half_float::half, char16_t>("to_unsigned"); 
    test_conversion<char32_t, half_float::half>("to_numeric");
*/
    std::cout << "============= demo completed ============\n";
    return 0;

}