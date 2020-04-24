

## Mini Library proposal:  to_signed() and to_unsigned() template methods

Qingfeng Xia

Senior Research Software Engineer

United Kingdom Atomic Energy Authority

Culham Science Centre

Abingdon Oxfordshire

OX14 3DB

United Kingdom

Email: qingfeng.xia@gmail.com

## 1. Revision History

## 2. Motivation

1. runtime arithmetic type conversion underflow/overflow safety in critical scenario,  

2. portable way to suppress compiler warning

https://thephd.github.io/portfolio/standard


3. CFFI:  javascript and matlab wrapping, there is only double type integer, even array index is double type, 

`narrow()`


#### Integral promotion

minus int to bool? specialized or disable!

float -> int, truncating not rounding

int -> float: IEEE rounding if EEE arithmetic is supported, 

## 3. Rationale

### fail-fast: for type conversion, for safety scientific computation.  

Java: does not care about integer overflow, during conversion?
https://stackoverflow.com/questions/3001836/how-does-java-handle-integer-underflows-and-overflows-and-how-would-you-check-fo

### C++ safe computation lib: 
decimal library, Java.BigInteger 

```cpp
bool is_safe_addition(uint32_t a, uint32_t b) {
    size_t a_bits=highestOneBitPosition(a), b_bits=highestOneBitPosition(b);
    return (a_bits<32 && b_bits<32);
}
```

CPU set status

## header-only usage

```cpp
#include "to_signed.h"

size_t f() { return 0xffffffffff; }
int i = std::to_signed<int>(f());
// std::overflow_error will throw here

```

## Implementation

function naming `std::to_string<>()`

### where to put
It is not compiling time check like functions in <type_traits>  

 <stddef> where size_t is defined, circular inclusion problem

double -> float will not use this function, but without report error

is `long long` and `long` same type no ?

### template parameter detection
return type must be the first template parameter, which must be specified. The second can be deduced.

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
1. integral and floating point
2. std::byte
3. enum:  magic_enum<E> to check max value
4. char types: unsigned integer

consider:  limited the target by `std::enable_if<std::is_signed<>::value, int>::value = 0` for `to_signed<>()`

###  Accepted source types
1. concern on floating point overflow

float to integer is possible, rounding to nearest if IEEE standard is supported

`numeric_limit<double>::is_iec559()`		true if the type adheres to IEC-559 / IEEE-754 standard.
> An IEC-559 type always has has_infinity, has_quiet_NaN and has_signaling_NaN set to true; And infinity, quiet_NaN and signaling_NaN return some non-zero value.

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

### not supported source types
1. pointer (unsigned long) but different type

`reinterpret_cast<int>(&var)` not safe, address is bigger than int
`static_cast<unsigned long>(&var);` only make sense to `unsigned long`

`std::to_address()`

### performance impact

 exception performance

##  tested platforms Compiler support

OS difference: LP64 or LLP64 (windows), 32bit or 64bit
CPU architecture, tested only on x86_64

Table tested compiler and platforms by CI
C++11 compiler


## Disclaimer of copyright 

Boost license
It is free for **open-source** projects and could be easily integrated with GitHub and BitBucket.

## References

[1] implicit conversion https://docs.microsoft.com/en-us/cpp/cpp/type-conversions-and-type-safety-modern-cpp?view=vs-2019

[2] 