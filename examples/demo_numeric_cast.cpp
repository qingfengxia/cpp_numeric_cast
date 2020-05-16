/***********************************************************
//              copyright Qingfeng Xia, 2020
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
************************************************************/

#include <iostream>
#include "demo.h"

enum E {E_0=0, E_9bit = 0x100, E_17bit = 0x10000, E_all_one=0xffffffff};
enum class TE{TE_0 =0, TE_9bit = 0x100, TE_17bit = 0x10000};
// TE_all_one=0xffffffff for strong typed enum

template<typename SourceType>
void test_enum(std::string fname, SourceType enum_value)
{
    std::cout << "=========" << "enum" << "==============\n";
    try
    {
        unsigned char uc = std::to_unsigned<unsigned char>(enum_value);
    }
    catch(std::overflow_error e)
    {
        std::cout << fname << "(" << type_name<SourceType>() << ") has error : " << e.what() << '\n'; 
    }
}

void test_byte()
{
#if __cplusplus >= 201703L
    std::cout << "=========" << "std::byte" << "==============\n";
    // implicitly conversion by assignment is not allowed from int to std::byte
    std::byte bb = static_cast<std::byte>(1);
    std::byte b{42};
    std::cout << std::to_integer<int>(b) << "\n";
    //std::cout << std::to_integer<int, std::byte>(std::byte{40}) << "\n";
    //std::numeric_limits<T> does not support `std::byte`
    //std::byte bc = std::to_unsigned<std::byte>(1);  
#endif
}

/// static_cast without messsage as the second parameter is only for C++17
void test_type_traits()
{
#if ! defined(_MSC_VER) || _MSC_VER >= 1900
    static_assert(std::is_same<int, int32_t>::value, "");

    // bool to unsigned or signal int are safe, is that must be 
    static_assert(static_cast<int>(true) == 1, "");  
    // int(true) == 1 failed on g++ 5

    //static_assert(std::is_same<long, long long>::value); // failed!
    // static_assert(std::is_same<int64_t, long>::value);
    // failed on macos 'std::is_same<long long, long>::value'

    //static_assert(std::is_same<int64_t, long>::value); // failed on macos
    static_assert(! std::numeric_limits<int>::has_infinity, ""); 
    
    static_assert(std::is_signed<double>::value, ""); // true for floating point
    //static_assert(std::is_signed<E>::value, ""); // failed for enum
    //static_assert(std::is_signed<bool>::value, ""); // failed for bool
    static_assert(std::is_unsigned<bool>::value, ""); // true for bool
    static_assert(std::is_integral<bool>::value, ""); // true for bool

    //static_assert(std::is_same<int*, void*>::value, "");

    static_assert(! std::is_arithmetic<int&>::value, "");
    static_assert(std::is_integral<char16_t>::value, ""); // true 
    static_assert(std::is_unsigned<char16_t>::value, ""); // true 
    //static_assert(std::is_same<char16_t, uint16_t>::value, "");
    //static_assert(std::is_same<char16_t, int16_t>::value, "");
#endif
}

#include "third-party/half.hpp"

void test_half()
{
    std::cout << "=========" << "half float" << "==============\n";
    using namespace half_float;
    half h1 = std::numeric_cast<half>(100);
    static_assert(std::is_numeric<half>::value, "");

    int8_t v1 = std::to_integer<int8_t>(half{1});
    try{
        int8_t v1 = std::to_integer<int8_t>(half{1000});
    }
    catch(std::runtime_error e)
    {
        std::cout << "`std::to_integer<int8_t>(half{1000})` error : " << e.what() << '\n'; 
    }
}

#if USE_BOOST_MULTIPRECISION
#include <boost/multiprecision/cpp_int.hpp>

// this  may shared the general test_conversion()
void test_boost_multiprecision()
{
   using namespace boost::multiprecision;

    std::cout << "=========" << "boost::multiprecision" << "==============\n";
   static_assert(std::detail::supports_arithmetic_operations<int128_t>::value, "");
   int128_t v = 1;
   //static_assert(std::is_arithmetic<int128_t>::value, "");

    int128_t i = std::numeric_cast<int128_t>(1);
    //int128_t i = std::numeric_cast<int128_t>(1.23);  // compiling error
    try{
        unsigned char uc = std::to_unsigned<unsigned char>(int128_t{1000});
        std::cout << "boost::multiprecision it128_t converted to ubyte " << uc << "\n";
    }
    catch(std::overflow_error e)
    {
        std::cout << " to_unsigned(boost_mp) error : " << e.what() << '\n'; 
    }

}
#endif

void test_implicit_conversion()
{
    const unsigned char uc = 1000.0;  // narrowing
    if(uc == UCHAR_MAX)
    {
        std::cout << "big floating point value truncated to UCHAR_MAX = " 
        << UCHAR_MAX << std::endl;
    }

    const unsigned char uc1 = 500;  // narrowing from int to uint8_t
    std::cout << "const unsigned char uc1 = 500 gives uc1 = " << int(uc1) << std::endl;
    if(uc1 == UCHAR_MAX)
    {
        std::cout << "big integer value truncated to UCHAR_MAX = " 
        << UCHAR_MAX << std::endl;
    }
}

void test_qualifier()
{
    /// volatile (deprecated) and reference 
    volatile int i_value = 1;
    unsigned char uc_v = std::to_integer<unsigned char>(i_value);
    volatile int& i_ref = i_value;
    unsigned char uc_vref = std::to_integer<unsigned char>(i_ref);

    /// const reference 
    const int c_value = 1;
    unsigned char uc_c = std::to_integer<unsigned char>(c_value);
    const int& c_ref = c_value;
    unsigned char uc_cref = std::to_integer<unsigned char>(c_ref);
}

int main()
{
    std::cout << std::boolalpha;

    test_conversion<double, unsigned int>("to_unsigned");
    test_conversion<double, uint64_t>("to_integer");
    test_conversion<double, int64_t>("numeric_cast");
    test_conversion<int64_t, bool>("to_integer");
    //test_conversion<int64_t, std::byte>("to_unsigned");
    test_conversion<int32_t, char16_t>("to_integer");

    float f = std::numeric_cast<float>(2);  // OK
    test_conversion<int, float>("numeric_cast");  // error, called `to_integer`
    test_conversion<uint32_t, int16_t>("to_integer");

    // todo: make test_enum a template function
    //test_enum("to_integer", E_9bit);
    test_enum("to_integer", TE::TE_9bit);
    //E e_cast = std::to_unsigned<E>(TE::TE_9bit);  // runtime_error error, why?

    test_byte();
    test_half();
    test_conversion<half_float::half, char16_t>("to_unsigned"); 
    test_conversion<char32_t, half_float::half>("numeric_cast");

#if USE_BOOST_MULTIPRECISION
    test_boost_multiprecision();  // not working
    //test_conversion<boost::multiprecision::int128_t, char16_t>("to_integer"); 
#endif
    test_implicit_conversion();

    static_assert(std::detail::_is_numeric<int>::value, "");
    test_qualifier();

    std::cout << "============= demo completed ============\n";
    return 0;

}