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


/// it is safe to inject into std namespace
namespace std {

namespace detail{

    template<class...> struct make_void { using type = void; };
    template<class... Ts> using void_t = typename make_void<Ts...>::type;
    
    template<class T, class = void>
    struct supports_arithmetic_operations : std::false_type {};
    
    template<class T>
    struct supports_arithmetic_operations<T, 
               void_t<decltype(std::declval<T>() + std::declval<T>()),
                      decltype(std::declval<T>() - std::declval<T>()),
                      decltype(std::declval<T>() * std::declval<T>()),
                      decltype(std::declval<T>() / std::declval<T>())>> 
           : std::true_type {};
}

    /// usage `int s = to_signed<int>(size_t_type_integer);`, by auto template type  derivation,
    /// enable_if<is_unsigned<S>, is_floating_point<S>, is_enum<S>>
    /// target type must be integer, bool, floating point must have sign
    /// target signed can be any arithmetic type, but should be signed integer
    /// to floating point is possible with lost precision
    template <typename T, typename S,
        typename std::enable_if<std::is_arithmetic<S>::value
        || detail::supports_arithmetic_operations<S>::value, int>::type = 0>
#if __cplusplus >= 201703L
    T constexpr to_signed(const S unsigned_int)
#else
    T to_signed(const S unsigned_int)
#endif

    {
        if (unsigned_int > std::numeric_limits<T>::max())
        {
            throw std::overflow_error(
                "input value overflow the target type");
        }

        return static_cast<T>(unsigned_int);
    }

    template <typename T, typename E, 
        typename std::enable_if<std::is_enum<E>::value, int>::type = 0>
    T to_signed(const E e)
    {
        typedef typename std::underlying_type<E>::type enum_under_type;
        int16_t b = static_cast<enum_under_type>(e);
        if (b > std::numeric_limits<T>::max())
        {
            throw std::overflow_error(
                "input value overflow the target type");
        }
        return static_cast<T>(e);
    }

#if __cplusplus >= 201703L
    template <typename T>
    T to_signed(const std::byte b)
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

    /// std::decay<>, remove reference, remove_cv()
    //template< typename T, typename U> std::string to_signed( const U& unsigned_int) 
    //{ return to_signed<T, U>(unsigned_int) ; }

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
    //{ return to_signed<T, U>(unsigned_int) ; }

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
    // enum_cast?
    template <typename E, typename S, 
        typename std::enable_if<std::is_enum<E>::value, int>::type = 0>
    E to_enum(S value)
    {
        using enum_under_type = typename std::underlying_type<E>::type;
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
        return E{};
    }

}