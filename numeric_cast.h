/***********************************************************
//              copyright Qingfeng Xia, 2020
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
************************************************************/

/**
* this is a header-only library
*/

#pragma once

#include <limits>
#include <stdexcept> // for std::overflow_error
#include <type_traits>

#if __cplusplus >= 201703L
#include <cstddef>
#endif

/// it is safe to inject into std namespace
namespace std {

namespace detail{

#if __cplusplus < 201703L
    // void_t is not defined until C++17
    template<class...> struct make_void { using type = void; };
    template<class... Ts> using void_t = typename make_void<Ts...>::type;
#endif

    template<class T, class = void>
    struct supports_arithmetic_operations : std::false_type {};
    
    template<class T>
    struct supports_arithmetic_operations<T, 
               void_t<decltype(std::declval<T>() + std::declval<T>()),
                      decltype(std::declval<T>() - std::declval<T>()),
                      decltype(std::declval<T>() * std::declval<T>()),
                      decltype(std::declval<T>() / std::declval<T>())>> 
           : std::true_type {};

    /// type_trait to test if numeric_limits<T> is instantiatable
    template<class T, class = void>
    struct _is_numeric : std::false_type {};
    
    template<class T>
    struct _is_numeric<T, 
               void_t<decltype(std::numeric_limits<T>::max()),
               decltype(std::numeric_limits<T>::min())>> 
           : std::true_type {};
 
    template <typename T, typename S,
        typename std::enable_if<std::is_arithmetic<S>::value
        || supports_arithmetic_operations<S>::value, int>::type = 0>
#if __cplusplus >= 201703L
    T constexpr numeric_cast(const S value)
#else
    T numeric_cast(const S value)
#endif
    {
        // todo:  warning: comparison between signed and unsigned integer expressions [-Wsign-compare]
        if (value > std::numeric_limits<T>::max())
        {
            throw std::overflow_error(
                "input value overflows the target type");
        }
        if (value < std::numeric_limits<T>::min())
        {
            // todo: message
            throw underflow_error(
                "input value underflows the target type");
        }
        return static_cast<T>(value);
    }

    template <typename T, typename S,
        typename std::enable_if<std::is_arithmetic<S>::value
        || supports_arithmetic_operations<S>::value, int>::type = 0>
    bool convertible(const S value) noexcept
    {
        if (value > std::numeric_limits<T>::max())
        {
            return false;
        }
        if (value < std::numeric_limits<T>::min())
        {
            return false;
        }
        return true;
    }

}

    /// convert to built-in arithmetic type and half, boost::multiprecision::int128_t
    template <typename T, typename S, 
        typename std::enable_if<std::is_arithmetic<T>::value
        || detail::supports_arithmetic_operations<T>::value, int>::type = 0>
    bool is_numeric_convertible(const S value) noexcept
    {
        return detail::convertible<T, S>(value);
    }

    /// specialization for enum source type
    template <typename T, typename E, 
        typename std::enable_if<std::is_enum<E>::value
        && std::is_integral<T>::value, int>::type = 0>
    bool is_numeric_convertible(const E e)
    {
        typedef typename std::underlying_type<E>::type enum_under_type;
        enum_under_type value = static_cast<enum_under_type>(e);
        return detail::convertible<T, E>(value);
    }

    /// convert to built-in arithmetic type and half, boost::multiprecision::int128_t
    template <typename T, typename S, 
        typename std::enable_if<std::is_arithmetic<T>::value
        || detail::supports_arithmetic_operations<T>::value, int>::type = 0>
    T numeric_cast(const S v)
    {
        return detail::numeric_cast<T, S>(v);
    }

    /// usage `int s = to_integer<int>(value);`, by auto template type derivation,
    /// target type must be integer, bool, floating point must have sign
    /// target signed can be any arithmetic type, but should be signed integer
    /// to floating point is possible with lost precision
    template <typename T, typename S, 
        typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
    T to_integer(const S v)
    {
        return detail::numeric_cast<T, S>(v);
    }

    template <typename T, typename E, 
        typename std::enable_if<std::is_enum<E>::value
        && std::is_integral<T>::value, int>::type = 0>
    T to_integer(const E e)
    {
        typedef typename std::underlying_type<E>::type enum_under_type;
        enum_under_type value = static_cast<enum_under_type>(e);
        if (value > std::numeric_limits<T>::max())
        {
            throw std::overflow_error(
                "input enum as an integer value overflows the target type");
        }
        if (value < std::numeric_limits<T>::min())
        {
            // todo: message
            throw underflow_error(
                "input enum as an integer value underflows the target type");
        }
        return static_cast<T>(value);
    }

// confliction with <cstddef> in C++17
#if 0 && __cplusplus >= 201703L
    template <typename T>  
    T to_integer(const std::byte b) throw
    {
        int16_t v = static_cast<int16_t>(b);
        if (v > std::numeric_limits<T>::max())
        {
            throw std::overflow_error(
                "input value overflow the target type");
        }
        return std::to_integer<T>(b);
    }

#endif

    /// usage `size_t s = to_unsigned(any_signed_integer);`, by auto template type  derivation
    /// target type must be unsigned integer,  not bool,  floating point must have sign
    /// source signed can be any arithetic type
    template  <typename T, typename S,
        typename enable_if<is_arithmetic<S>::value
             || detail::supports_arithmetic_operations<S>::value, int>::type = 0>
    T to_unsigned(const S signed_value)
    {
        if (signed_value > numeric_limits<T>::max())
        {
            throw overflow_error(
                "input value overflow the target type");
        }
        if (signed_value < 0)
        {
            throw underflow_error(
                "signed value less than zero should not be converted to unsigned integer");
        }
        return static_cast<T>(signed_value);
    }
    //template< typename T, typename U> std::string to_unsigned( const U& unsigned_int) 
    //{ return to_integer<T, U>(unsigned_int) ; }


    template  <typename T, typename E,
        typename enable_if<is_enum<E>::value, int>::type = 0>
    T to_unsigned(const E enum_value)
    {
        using enum_under_type = typename underlying_type<E>::type;
        enum_under_type value = static_cast<enum_under_type>(enum_value);
        if (value > numeric_limits<T>::max())
        {
            throw overflow_error(
                "input value overflow the target type");
        }
        if (value < 0)
        {
            throw underflow_error(
                "signed value less than zero should not be converted to unsigned integer");
        }
        return static_cast<T>(value);
    }

#if __cplusplus >= 201703L
    template <typename S, // enum and arithmetic
        typename std::enable_if<std::is_arithmetic<S>::value 
        || std::is_enum<S>::value, int>::type = 0>
    std::byte to_unsigned(const S signed_value)
    {
        if (signed_value > std::numeric_limits<unsigned char>::max())
        {
            throw std::overflow_error(
                "input value overflow the target type std::byte (unsigned char)");
        }
        if (signed_value < 0)
        {
            throw std::underflow_error(
                "signed value less than zero should not be converted to std::byte");
        }
        return std::byte{signed_value};
    }

#endif
    // a new name as enum_cast?
    template <typename E, typename S, 
        typename std::enable_if<std::is_enum<E>::value, int>::type = 0>
    E to_enum(const S value)
    {
        using enum_under_type = typename std::underlying_type<E>::type;
        const enum_under_type enum_value = to_integer<enum_under_type>(value);
        /*
        if (value > std::numeric_limits<enum_under_type>::max())
        {
            throw std::overflow_error(
                "input value overflow the target type std::byte (unsigned char)");
        }
        if (value < std::numeric_limits<enum_under_type>::min())
        {
            throw std::underflow_error(
                "signed value less than zero should not be converted to std::byte");
        }
        */
        return E{enum_value};  /// enum is not validated for existence!
    }

}