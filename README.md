

## Mini Library proposal:  to_signed() and to_unsigned() template methods

Qingfeng Xia, Copyright 2020
Boost library license

## 1. Revision History

## 2. Motivation

1. a portable way to suppress compiler warning on conversion

From size_t, returned by STL container lib to signed integer for some arithmetic operation is a common operation, but there is compiler warning. 
``` cpp
int i = myVector.size() - 100;
```

`int i = static_cast<int> ( myVector.size() - 100);` can suppress this warning, but
if `myVector.size() -100 > INT_MAX` in big data application, then the result is not what we want.

Another example, from signed integer to STL container index

```cpp
#include <iostream>
#include <vector>
int f() { return -1; };
int main()
{
    auto myVector = std::vector<int> {1, 2, 3};
    int i = f(); // runtime value can be -1
    std::cout << "myVector[i] = " << myVector[i] << std::endl;
    return 0;
}
```

2. to throw runtime exception for arithmetic type conversion underflow/overflow  in critical scenario. 

```cpp
// there is a compiler warning "overflow" for constant assignment 
target_implicitly_converted = std::numeric_limits<TargetType>::max() + 1.0;
std::cout << "implicitly assigned an overflow value to unsigned int  = " 
    << target_implicitly_converted<< '\n';
// for function return not even a warning
target_implicitly_converted = generate_overflow_value();
```

3. foreign function interface (FFI)
For example, javascript has only double type integer, even array index is double type. It must be convert to unsigned int in C/C++ side.  It is kind of unsafe `narrow`, in contrast of from safe numeric promotion.

#### Integral promotion and conversion

see external link: 
uint32_t to int64_t is safe, the latter one has range covering the previous one, conersion happens without compiler warning. 

uint32_t to int32_t is not safe, overflow can happen. it is conversion with compiler warning.

float -> int, if float_value > INT_MAX, set the int_value to INT_MAX.
int -> float: IEEE rounding if EEE arithmetic is supported, 

## 3. Rationale

### fail-fast: for type conversion, for safety scientific computation.  

C# `checked` integer overflow. 

```cs
byte value = 240;
try
{
    checked
    {
        value += 24;
    }
}
catch (OverflowException e)
{
    // handle overflow, get overflow count via % etc.
}
```

https://stackoverflow.com/questions/3001836/how-does-java-handle-integer-underflows-and-overflows-and-how-would-you-check-fo

### C++ safe arithmetic computation lib

The easy way to avoid overflow is to use number types with unlimited value range like python3 integer,  at the cost of longer computation time. CPU are designed to compile arithmetic operation on fixed bit length data quickly, usually in one CPU pulse time. 

C++ does not have multiprecision integer support like Java or C# BigInteger,  but boost has `boost::multiprecision::cpp_int<>` has the template parameter to control runtime exception **checked** or not. For the checked types, `std::overflow_error` or `range_error` will be thrown at runtime, instead of sliently set the result as the modulo to the max value. see more details <https://www.boost.org/doc/libs/1_71_0/libs/multiprecision/doc/html/boost_multiprecision/tut/ints/cpp_int.html>

There are alias to the fixed bit length and unchecked `cpp_int<>` template class, `int128_t, uint128_t, ..., uint1024_t`, those are unchecked, which have the same hehavriour as C/c++ build type `uint32_t`. Meanwhile, checked version are named as `checked_uint128_t`. 

Boost multiprecision library also have binary and decimal floating point number such as `float128`

### Hardware support of overflow

CPU (x86) will set status register for integer overflow/underflow, however, it is ignored in computation normally. C++ compiler may has feature to react on this condition as "-ftrapv", which is turned off by default for performance reason. 
see the example code at <https://gist.github.com/mastbaum/1004768>

Similarly,  floating point computation hardware (FPU) may signal for NAN result. This is done by C standard library `<fenv.h>`, floating-point status flags `fexcept_t` (it is not C++ exception class)	will be set and can be tested by bit and operation, see example usage <https://en.cppreference.com/w/cpp/numeric/fenv/feraiseexcept>. Alternatively, those status can be translated into runtime C++ exception if enabled by compiler specific switch see <https://en.cppreference.com/w/cpp/numeric/fenv>

There are compiler extensions that may be used to generate C++ exceptions automatically whenever a floating-point exception is raised:


## header-only usage

```cpp
#include "to_signed.h"

size_t f() { return 0xffffffffff; }
int i = std::to_signed<int>(f());
// std::overflow_error will throw here

```

## Implementation

function naming follows `std::to_string<T>()`

### which standard header those functions should go?

`<type_traits>  and <>` are used to implement those function.  

`<stddef>` where `size_t` is defined, if it will not cause circular inclusion.

### template parameter detection
The return type must be the first template parameter, which must be specified. The second can be deduced from input parameter type.

```cpp
template <typename R, typename P>
R f(P i)
{
    R sv{};
    return sv;
}

int main()
{
    int r = f<int>(1.2);
}
```

### Target types
1. integral , floating point , char types: unsigned integer,  any class support `std::numeric_limits<T>`

Customized classes, half float as well as built-in floating point types are signed types. boost multiple precision types have both signed and unsigned version depends on template parameter given.

2. `std::byte` introduced in C++17
template specialization has been implemented to support
`TargetType t = to_signed<TargetType>(std::byte b)` 
`std::byte b = to_unsigned<SourceType>(SourceType value)`

3. enum and enum class (c++11):   
 use different name `to_enum<>()` instead of `to_unsigned()`

 enum's default underlying integer type is `unsigned int` in C++
`to_enum<>()` should also translate different enum types.

Consider using `magic_enum<E>` to check if an enum value valid, instead of checking overflow by `std::numeric_limits<underlying_integer_type>`

> enum value is valid in C++ if it falls in range [A, B], which is defined by the standard rule below. So in case of enum X { A = 1, B = 3 }, the value of 2 is considered a valid enum value.  see more "7.2/6 of standard"

4. bool:  a special arithmetic unsigned type in C++
`is_unsigned<bool>::value == true`  and `is_arithmetic<bool>::value == true`
The bool type should NOT used in `to_unsigned`. Meanwhile, common types to bool conversion is well defined in C++ standard [ref??]()
`explicit bool operator ()`

consider:  limited the target by `std::enable_if<std::is_signed<>::value, int>::value = 0` for `to_signed<>()`

###  Accepted source types

built-in types `is_arithmetic<T>` or user defined class support arithmetic operations.

There are five integer types, is `long long` and `long` same type? NO on ubuntu x86-64. 

1. floating point to integer

double -> float will not use this function, but without report error
float to integer is possible, rounding to nearest if IEEE standard is supported. 

`numeric_limit<double>::is_iec559()` is true if the type adheres to IEC-559 / IEEE-754 standard.
> An IEC-559 type always has has_infinity, has_quiet_NaN and has_signaling_NaN set to true; And infinity, quiet_NaN and signaling_NaN return some non-zero value.

`is_arithmetic<>` return true type for customized type is not strait-forward, however, a new type_trait `support_arithmetic<>` can be declared as suggested
<https://stackoverflow.com/questions/26434128/how-to-make-is-arithmeticmyclassvalue-to-be-true>

bool to unsigned or signal int are safe, is that must be 
 static_assert(int(true) == 1);

2. char types: `std::is_integral<char16_t>::value  == true`
They are included in <limits>, so they must be implemented.

only for `std::is_arithmetic<char16_t>::value`
char16_t is not the same type as `uint16_t` or `int16_t`, it is a new type

```cpp
char16_t
char32_t
wchar_t
char8_t since C++20
```

3. bool type is kind of special unsigned integer type
`std::is_unsigned<bool>::value == true`

4. std::byte
`std::to_integer<TargetIntType>(std::byte b)`

6. enum  `std::byte` is a `enum class`,    2 step cast
  **static_cast**,  cast if conversion exists, may and may not have compiler warning

  **dynamic_cast**, for safe, runtime-checked casts of pointer-to-base to pointer-to-derived.

  constness: input parameter as const? always create a new on the left. rvalue

7.  half precision float, standardized by IEEE754
An open source version of half implemented has been incorporated into this project. 
`numeric_limits<T>`

`to_unsigned<>` is prefered over `half_cast<>` for the same API to deal with all source type. 

8. boost: multiple precision integer and decimal
`cpp_int<>` and `cpp_dec_float<>` can `std::range_error` for checked version

### not supported source types
1. pointer (has the same byte size as `unsigned long int`) but it is a different type.

`reinterpret_cast<int>(&var)` not safe, address is bigger than int
`static_cast<unsigned long>(&var);` only make sense to `unsigned long`

`std::to_address()`

### performance impact

exception overhead is regardly insignificant
C++17 `if constexpr ()` may reduce runtime 

##  tested platforms Compiler support

Different OS: LP64 or LLP64 (windows), only 64bit
CPU architecture, tested only on x86_64

Table tested compiler and platforms by CI, using C++11 compiler


## Disclaimer and copyright 

This project (code) is a personal contribution in personal time, it is not a work related/sponsored by my employer.

Codes in this repository (except for lib in third-party lib) are licensed in
Boost library license. It is free for open source and close source project.

==Contact== 

Qingfeng Xia
Senior Research Software Engineer
United Kingdom Atomic Energy Authority
Culham Science Centre, Abingdon Oxfordshire 
OX14 3DB
United Kingdom
Email: qingfeng.xia(a)gmail

## References

[1] implicit conversion https://docs.microsoft.com/en-us/cpp/cpp/type-conversions-and-type-safety-modern-cpp?view=vs-2019

[2] to add more