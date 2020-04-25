/***********************************************************
//              copyright Qingfeng Xia, 2020
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
************************************************************/

#include <iostream>
#include <typeinfo>
#include <string>
#include <map>
#include <cstddef>

#include "to_signed.h"

// typeid().name() of g++ does not given full type name
#include "third-party/type_name.h"

//template <typename ReturnedType, typename IntType>
template <typename IntType>
double generate_overflow_value()
{
    return std::numeric_limits<IntType>::max() + 1.0;
}

//template <typename ReturnedType, typename IntType>
template <typename IntType>
double generate_underflow_value()
{
    return std::numeric_limits<IntType>::min() - 1.0;
}

template <typename SourceType, typename TargetType>
void test_conversion_inf_nan(std::string fn)
{
    TargetType target_implicitly_converted = 0;  // -Wall
    std::cout << "quite nan: " << std::numeric_limits<TargetType>::quiet_NaN() << '\n'; // just zero
    std::cout << "int  infinity: " << std::numeric_limits<TargetType>::infinity() << '\n';  // just zero

    const auto& source_name = type_name<SourceType>();
    const auto& target_name = type_name<TargetType>();
    std::string fname = std::string( fn + "<") + target_name + ">(" + source_name + ")" ;

    SourceType inf_value = std::numeric_limits<SourceType>::infinity();
    SourceType nan_value = std::numeric_limits<SourceType>::quiet_NaN();

    target_implicitly_converted = inf_value;
    std::cout << "implicitly convert floating point infinity  to " << target_name << " = "
        << target_implicitly_converted<< '\n'; 

    target_implicitly_converted = nan_value;
    std::cout << "implicitly convert floating point NaN to " << target_name << " = "
        << target_implicitly_converted<< '\n';

    try {
        TargetType target_cast_inf = TargetType();
        TargetType target_cast_nan = TargetType();
        if(fn == "to_unsigned")
        {
            target_cast_inf = std::to_unsigned<TargetType, SourceType>(inf_value);
            target_cast_nan = std::to_unsigned<TargetType, SourceType>(nan_value);
        }
        else
        {
            target_cast_inf = std::to_signed<TargetType, SourceType>(inf_value);
            target_cast_nan = std::to_signed<TargetType, SourceType>(nan_value);
        }
        
        std::cout << fname << " inf  = " << target_cast_inf<< '\n'; 
        std::cout << fname << " nan  = " << target_cast_nan<< '\n'; 
    }
    // std::runtime_error to catch either
    catch (std::overflow_error& e)
    {
        std::cout << fname << " has error : " << e.what() << '\n'; 
    }
    catch (std::underflow_error& e)
    {
        std::cout << fname << " has error : " << e.what() << '\n'; 
    }
}

template <typename SourceType, typename TargetType>
void test_conversion(std::string fn)
{
    SourceType stype_max = std::numeric_limits<SourceType>::max();
    SourceType stype_min = std::numeric_limits<SourceType>::min();
    TargetType target_implicitly_converted = 0; 

    const auto& source_name = type_name<SourceType>();
    const auto& target_name = type_name<TargetType>();
    std::string fname = std::string( fn + "<") + target_name + ">(" + source_name + ")" ;

    std::cout << "=========" << fname << "==============\n";
    // code below causes compiling error for half, boost types
    const std::map<std::string, SourceType> values{
        {source_name + std::string("max"), stype_max}, 
        {source_name  + std::string("min"), stype_min},
        {"generated overflow value", generate_overflow_value<TargetType>()},
        {"generated underflow value", generate_underflow_value<TargetType>()}
        };

    for(const auto& p: values)
    {
        TargetType target_implicitly_converted = 0; 
        TargetType target_cast = 0;
        SourceType v = p.second;
        if(not std::is_class<SourceType>::value)
        {
        target_implicitly_converted = v;
        std::cout << "implicitly convert  value to unsigned int  = " 
            << target_implicitly_converted<< '\n';
        }

        try {
            if(fn == "to_unsigned")
                target_cast = std::to_unsigned<TargetType, SourceType>(v); 
            else if (fn == "to_signed")
            {
                target_cast = std::to_signed<TargetType, SourceType>(v); 
            }
            else
            {
                /* code */
            }
            
            //unsigned int target_cast_2 = std::to_unsigned(d); // couldn't deduce template parameter ‘S’
            std::cout << fname  << "= " << target_cast << '\n'; 
        }
        catch (std::runtime_error& e)
        {
            std::cout << fname << " has error : " << e.what() << '\n'; 
        }
    }

    //if(std::numeric_limits<SourceType>::has_infinity)
    if(std::is_floating_point<SourceType>::value)
    {
        test_conversion_inf_nan<SourceType,  TargetType>(fn);
    }

    // there is a warning for 1.0e12 constant assignment, for function return not even a warning
    target_implicitly_converted = std::numeric_limits<TargetType>::max() + 1.0;
    std::cout << "implicitly convert floating point overflow value to unsigned int  = " 
        << target_implicitly_converted<< '\n';

    target_implicitly_converted = std::numeric_limits<TargetType>::min() - 1.0;
    std::cout << "implicitly convert floating point underflow value to unsigned int  = " 
        << target_implicitly_converted<< '\n';

}

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
    //std::cout << std::to_signed<int, std::byte>(std::byte{40}) << "\n";
    //std::numeric_limits<T> does not support `std::byte`
    //std::byte bc = std::to_unsigned<std::byte>(1);  
#endif
}

void test_type()
{
    static_assert(std::is_same<int, int32_t>::value);

    //static_assert(std::is_same<long, long long>::value); // failed!
    // static_assert(std::is_same<int64_t, long>::value);
    // failed on macos 'std::is_same<long long, long>::value'

    static_assert(std::is_same<int64_t, long>::value);
    static_assert(not std::numeric_limits<int>::has_infinity); 
    
    static_assert(std::is_signed<double>::value); // true for floating point
    //static_assert(std::is_signed<E>::value); // failed for enum
    //static_assert(std::is_signed<bool>::value); // failed for bool
    static_assert(std::is_unsigned<bool>::value); // true for bool
    static_assert(std::is_integral<bool>::value); // true for bool

    //static_assert(std::is_same<int*, void*>::value);

    static_assert(std::is_integral<char16_t>::value); // true 
    static_assert(std::is_unsigned<char16_t>::value); // true 
    //static_assert(std::is_same<char16_t, uint16_t>::value);
    //static_assert(std::is_same<char16_t, int16_t>::value);
}

#include "third-party/half.hpp"

void test_half()
{
    using namespace half_float;
   //static_assert(std::is_arithmetic<half_float::half>::value);
    int8_t v1 = std::to_signed<int8_t>(half{1});
    try{
        int8_t v1 = std::to_signed<int8_t>(half{1000});
    }
    catch(std::runtime_error e)
    {
        std::cout << "`std::to_signed<int8_t>(half{1000})` error : " << e.what() << '\n'; 
    }
}

#if __cplusplus >= 201703L
// `__has_include()` is only supported by C++17
#if __has_include("boost/multiprecision/cpp_int.hpp")
#include <boost/multiprecision/cpp_int.hpp>

// this  may shared the general test_conversion()
void test_boost_multiprecision()
{
   using namespace boost::multiprecision;

   static_assert(std::detail::supports_arithmetic_operations<int128_t>::value);
   int128_t v = 1;
   //static_assert(std::is_arithmetic<int128_t>::value);

   // int i = std::to_integer<int>(1.2);
    try{
        unsigned char uc = std::to_unsigned<unsigned char>(int128_t{1}); // todo
    }
    catch(std::overflow_error e)
    {
        std::cout << " to_unsigned(enum) error : " << e.what() << '\n'; 
    }

}
#endif
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

int main()
{
    std::cout << std::boolalpha;

    test_conversion<double, unsigned int>("to_unsigned");
    test_conversion<double, uint64_t>("to_unsigned");
    test_conversion<int64_t, bool>("to_unsigned");
    // bool to unsigned or signal int are safe, is that must be 
    static_assert(int(true) == 1);
    //test_conversion<int64_t, std::byte>("to_unsigned");

    test_conversion<uint64_t, float>("to_unsigned");
    test_conversion<double, int64_t>("to_signed");
    test_conversion<uint32_t, int16_t>("to_signed");

    test_conversion<int32_t, char16_t>("to_unsigned");


    //test_enum("to_signed", E_9bit);
    test_enum("to_signed", TE::TE_9bit);
    //E e_cast = std::to_unsigned<E>(TE::TE_9bit);  // runtime_error error, why?

    test_byte();
    test_half();
    //test_conversion<half_float::half, char16_t>("to_unsigned");
#if __cplusplus >= 201703L
    test_boost_multiprecision();  // not working
#endif
    test_implicit_conversion();
    //test_conversion<boost::multiprecision::int128_t, char16_t>("to_unsigned");  

    int i_value = 1;
    int& i_ref = i_value;
    unsigned char uc = std::to_unsigned<unsigned char>(i_ref);

    std::cout << "============= demo completed ============\n";
    return 0;

}