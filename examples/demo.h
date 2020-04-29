/***********************************************************
//              copyright Qingfeng Xia, 2020
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
************************************************************/
#pragma once

#include <iostream>

#include <string>
#include <map>
#include <cstddef>

#include "to_integer.h"

#include <typeinfo>
// typeid().name() of g++ does not given full type name
#include "third-party/type_name.h"

//template <typename ReturnedType, typename IntType>
template <typename SourceType>
double generate_overflow_value()
{
    return std::numeric_limits<SourceType>::max() + 1.0;
}

//template <typename ReturnedType, typename IntType>
template <typename SourceType>
double generate_underflow_value()
{
    return std::numeric_limits<SourceType>::min() - 1.0;
}

template <typename SourceType, typename TargetType>
void test_conversion_inf_nan(std::string fn)
{
    TargetType target_implicitly_converted;
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
            target_cast_inf = std::to_numeric<TargetType, SourceType>(inf_value);
            target_cast_nan = std::to_numeric<TargetType, SourceType>(nan_value);
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

/// SourceType as the first template parameter, target as the second
template <typename SourceType, typename TargetType>
void test_conversion(const std::string fn)
{
    SourceType stype_max = std::numeric_limits<SourceType>::max();
    SourceType stype_min = std::numeric_limits<SourceType>::min();
    TargetType target_implicitly_converted{0};   // float half class does not support assignment

    const auto& source_name = type_name<SourceType>();
    const auto& target_name = type_name<TargetType>();
    std::string fname = std::string( fn + "<") + target_name + ">(" + source_name + ")" ;

    std::cout << "=========" << fname << "==============\n";
    // values map creation by {{}, {}} causes compiling error for half, boost types
    std::map<std::string, SourceType> values;
    values[source_name + std::string("max")] = stype_max; 
    values[source_name  + std::string("min")] = stype_min;
    values["generated overflow value"] = generate_overflow_value<TargetType>();
    values["generated underflow value"] = generate_underflow_value<TargetType>();

    for(const auto& p: values)
    {
        TargetType target_implicitly_converted{0}; 
        TargetType target_cast{0}; 
        SourceType v = p.second;
        if(not std::is_class<SourceType>::value)
        {
        target_implicitly_converted = v;
        std::cout << "implicitly convert value to " << target_name << " = " 
            << target_implicitly_converted<< '\n';
        }

        try {
            if(fn == "to_unsigned")
            {
                target_cast = std::to_unsigned<TargetType, SourceType>(v); 
            }
            else if (fn == "to_integer")
            {
                target_cast = std::to_numeric<TargetType, SourceType>(v); // todo: int->double
            }
            else if (fn == "to_numeric")
            {
                target_cast = std::to_numeric<TargetType, SourceType>(v); 
            }
            else
            {
                auto msg = "test function" + fname + "is not recognized";
                throw std::runtime_error(msg.c_str());
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

#if 0
    // there is a warning for 1.0e12 constant assignment, for function return not even a warning
    target_implicitly_converted = std::numeric_limits<TargetType>::max() + 1.0;
    std::cout << "implicitly convert floating point overflow value to unsigned int  = " 
        << target_implicitly_converted<< '\n';

    target_implicitly_converted = std::numeric_limits<TargetType>::min() - 1.0;
    std::cout << "implicitly convert floating point underflow value to unsigned int  = " 
        << target_implicitly_converted<< '\n';
#endif
}