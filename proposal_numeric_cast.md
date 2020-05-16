#  Runtime safe numeric type conversion

Project: Programming Language C++
Audience: Study Group 6 (Numerics), Library Evolution Working Group Incubator

Qingfeng Xia, Copyright 2020


## 0. Revision History

## 1. Introduction

Proposed a few helper functions to make numeric conversion safer.
```cpp

// type trait for both built in numeric types and user defined type like half float
is_numeric<T>();

// throwable dynamic numeric cast
numeric_cast()  // a simplified but should works like boost::numeric::numeric_cast
// note: after the author reinvented the wheel, `boost::numeric::numeric_cast` was found to be more feature complete with trait and policy. 

to_index()  // to size_t

// type traits to check if built in types and user defined types support numeric_cast
is_numeric<T>::value

// check if the cast may cause runtime error
bool convertible<TargetType>(SourceType) noexcept
// can  be named as  `is_numeric_convertible()`
```

```c++
// from numeric types to enum with runtime overflow check
// javascript has only 1 numeric type double, 
// safe cast from double to C++ enum may be useful
to_enum()  

```

## 2. Motivation

1. A portable way to suppress compiler warning on conversion

Conversion from unsigned integer of `size_t` type, returned by STL containers. to signed integer for some arithmetic operation is a common operation, but there is compiler warning. 

[Compiler Warning (level 4) C4245, 'conversion' : conversion from 'type1' to 'type2', signed/unsigned mismatch](https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4245?view=vs-2019)

Compiler should issue some warning for the code snippet below, if the warning switch has been turned on.

``` cpp
#include <vector>
#include <iostream>

int main() {
// todo add the demo code here
}
```

In C++11, `static_cast` can suppress this warning, e.g. `int i = static_cast<int> ( myVector.size() - 100);`  but if `myVector.size() - 100 > INT_MAX` in big data application, then the result is not what we want.

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

`std::make_signed` used in meta programming, but not relevant here. 
https://stackoverflow.com/questions/13150449/efficient-unsigned-to-signed-cast-avoiding-implementation-defined-behavior


2. to throw runtime exception for arithmetic type conversion underflow/overflow  in critical scenarios. 

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

### Background
#### Integral promotion and conversion

see external link: 
`uint32_t` to `int64_t` is safe, the latter one has range covering the previous one, conersion happens without compiler warning. 

for the expression, the automatically conversion from `int32_t` to `uint32_t` is not safe. Minus integer is converted into a very big positive number. Converted back to signed integer can be arihtmetic correctly, but the leave expression result as unsigned type is not ideal.

 assigning `uint32_t` value to `int32_t` is not safe, overflow can happen. it is implicite conversion with compiler warning.

float -> int, if float_value > INT_MAX, set the int_value to INT_MAX.
int -> float: IEEE rounding if EEE arithmetic is supported, 


### Boost safe numerics

```cpp
checked_result<R>
Checked Arithmetic
interval<R>
safe_compare<T, U>
```

## 3. Rationale

Compiling time `is_narrow_convertible<>` type traits, this is a runtime check.
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p0870r2.html

### boost safe numerics
`safe<T>`
> library absolutely guarantees that no integer arithmetic expression will yield incorrect results. 
https://www.boost.org/doc/libs/1_73_0/libs/safe_numerics/doc/html/index.html


"a type T is Numeric if there exists a specialization of `std::numeric_limits<T>`"
`to_value` or `numeric_cast`
`IndexType to_index<ContainerType, SourceType>(SourceType v)`


```cpp
typedef boost::numeric::converter<int,double> Double2Int ;
int x = Double2Int::convert(2.0);
```
https://www.boost.org/doc/libs/1_73_0/libs/numeric/conversion/doc/html/index.html

### fail-fast: for type conversion, for safety scientific computation.  

C# has a keyword `checked` to check integer overflow at runtime. 

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

## Existing implementation

### Boost numeric conversion 

[This header-only library](https://github.com/boostorg/numeric_conversion) dates back to 2001, and it supports more compilers, perhaps through `boost/detail/workaround.hpp`. 

In addition to conversion for int-float, mixed sign, mixed built-in and user-defined types, this lib has advanced features like: `round_policy, overflow_policy and range_checking_policy`. 

[Usage](https://www.boost.org/doc/libs/1_72_0/libs/numeric/conversion/doc/html/boost_numericconversion/improved_numeric_cast__.html):  `unsigned int i=numeric_cast<unsigned int>(f);`,  is identical to the proposed implementation.  If failed, `boost::numeric::bad_numeric_cast`  derived exception will be thrown, it could be `boost::numeric::positive_overflow` or `boost::numeric::negative_overflow`


```cpp
template <typename Target, typename Source> 
inline Target numeric_cast( Source arg )
{
    typedef numeric::conversion_traits<Target, Source>   conv_traits;
    typedef numeric::numeric_cast_traits<Target, Source> cast_traits;
    typedef boost::numeric::converter
        <
            Target,
            Source, 
            conv_traits,
            typename cast_traits::overflow_policy, 
            typename cast_traits::rounding_policy, 
            boost::numeric::raw_converter< conv_traits >,
            typename cast_traits::range_checking_policy
        > converter;
    return converter::convert(arg);
}
```

The implementation use on boost MPL. User defined types needs implementation `std::numeric_limits<>`, and more type trait is needed? 

The `float_half::half` has implements `std::numeric_limits<>`, but failed to throw an exception, reason is unknown.
```
        REQUIRE_THROWS_AS( boost::numeric_cast<int8_t>(half{1000}) == 0, boost::numeric::bad_numeric_cast);
```

#### User-defined type support


The default rounding policy: 

### Other (mainly limited to integral conversion)

`gsl::narrow` of [GNU Science library]()

`wil::safe_cast` of [Windows Implementation Libraries (WIL)](https://github.com/microsoft/wil/wiki/safe_cast)

## Implementation


### Try it now:  header-only single file library

[download numeric_cast.h](./numeric_cast.h)

```cpp
#include "numeric_cast.h"

size_t f() { return 0xffffffffff; }
int i = std::numeric_cast<int>(f());
// std::overflow_error will throw here

```

Boost sign function
https://www.boost.org/doc/libs/1_73_0/libs/math/doc/html/math_toolkit/sign_functions.html
 
### Single header file (needs C++11)

The actual conversion is done by  **static_cast<>**  

 **static_cast**,  cast if conversion exists, may and may not have compiler warning

 **dynamic_cast**, for safe, runtime-checked casts of pointer-to-base to pointer-to-derived.

The source value will not be modified, this function will always create a new value of the target type.

Header files required:
`<type_traits> ` to limit source type to convert, so C++11 is a minimum requirement
`<limits>`  for overflow detection, get the target type `min()` and `max()`
`<stdexcept>`  for standard overflow exceptions 

### Function naming

1. `std::numeric_cast<>()`, is a general method for numeric types conversion. 
```cpp
std::is_arithmetic<T>::value || detail::supports_arithmetic_operations<T>::value
```

2. `std::to_integer<>()`: reuse the name `std::to_integer<>(std::byte)`. 
From `std::byte` to integer is safe except for to `int8_t`. 
```cpp
  template<typename _IntegerType>
    constexpr _IntegerType
    to_integer(__byte_op_t<_IntegerType> __b) noexcept
    { return _IntegerType(__b); }
```
It is proposed to modify this `std::to_integer<>(std::byte)` to check for the `int8_t` special case.

function naming follows `std::to_string<T>()`

3. `to_enum<>()` or `enum_cast<>()`


### which standard header those functions should go?

`<numeric>`

### Template parameter detection
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

why `boost::math::round` can not provide a template parameter for the return type?

https://www.boost.org/doc/libs/1_73_0/libs/math/doc/html/math_toolkit/rounding/round.html

### Target types
1. integral , floating point , char types: unsigned integer,  any class support `std::numeric_limits<T>`

Customized classes, half float as well as built-in floating point types are signed types. boost multiple precision types have both signed and unsigned version depends on template parameter given.

2. `std::byte` introduced in C++17
template specialization has been implemented to support
`TargetType t = to_integer<TargetType>(std::byte b)` 
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

consider:  limited the target by `std::enable_if<std::is_signed<>::value, int>::value = 0` for `to_integer<>()`

###  Accepted source types

built-in types `is_arithmetic<T>` or user defined class support arithmetic operations.

There are five signed integer types, `long long` and `long` are different type? at least on ubuntu x86-64. 

`static_assert(not std::is_arithmetic<int&>::value);`  it is illegal to supply reference as the target type. In source type/input parameter type can be the value type or reference to the value type with and without volatile and const qualifier.

```cpp
    const int c_value = 1;
    unsigned char uc_c = std::to_integer<unsigned char>(c_value);
    const int& c_ref = c_value;
    unsigned char uc_cref = std::to_integer<unsigned char>(c_ref);
```

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

`std::to_address()` works for raw pointer and smart pointer classes


### Performance impact

exception overhead is regarded insignificant in modern C++.[ref needed]

C++17 `if constexpr ()` may reduce runtime overhead.

inline template function, yes!


##  Tested platforms Compiler support

Different OS: LP64 or LLP64 (windows), only 64bit
CPU architecture, tested only on x86_64, but it is possible to test on ARM cpu on one CI system.

Table 1: tested compiler and platforms by CI, using C++11 compiler

| Platform                      | Description | Compiler version | Result |
| ----------------------------- | ----------- | ---------------- | ------ |
| MacOS Xcode Travis CI         | C++14       | Apple clang      |        |
| Ubuntu 18.04 64bit  Travis CI | C++17       | G++ 8            |        |
| Ubuntu 16.04 64bit  Travis CI | C++11       | G++ 5            |        |
| Windows Azure pipeline CI     | C++14       | visual studio 2015 |        |
| Windows Azure pipeline CI     | C++17       | visual studio 2019 |        |

Visual studio C++ version macro
https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=vs-2019

## Disclaimer and copyright 

This project (code) is a personal contribution in personal time, it is not a work sponsored by my employer. If this feature is can be submit as a c++ proposal, the author may seek sponsorship from the employer.

Codes in this repository (except for lib in third-party lib) are licensed in
[Boost Software License](https://www.boost.org/users/license.html). It is free for open source and close source project.

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

[2] [A proposal for a type trait to detect narrowing conversions](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p0870r2.html)