# explicit keyword to prevent function parameter implicit conversion

very early draft for discussion

By Qingfeng Xia 2020


## Motivation
C++ compiler has `explicit` keyword as for constructor and type operator, such as bool <https://en.cppreference.com/w/cpp/language/explicit>, to stop implicit conversion via assignment.

The following code looks similar, but the result is distinct. 
```cpp
template <typename T>  void print_vector(std::vector<T> v);
// ...
std::vector<double> v(2.5);  
print_vector(v);  
std::vector<double> v2({2.5});  
print_vector(v2);  
```

The full example can be found [here](./examples/demo_explicit.cpp).  NO compiler warning even if `-Wall` switch is on. Some types may have several constructors, overload resolution is too complicate for novice users to understand. 

There is no keyword to stop implicit conversion of function parameters.

### `explicit` keyword in other languages

#### C#
C# has [`implicit` and also `explicit` keywords](https://docs.microsoft.com/en-us/dotnet/csharp/language-reference/operators/user-defined-conversion-operators)
C# implicit conversion only allow numeric promotion, like from  `unsigned int` to `long long`

```c#
  
    public static implicit operator byte(Digit d) => d.digit;
    public static explicit operator Digit(byte b) => new Digit(b);
```
The `explicit` keyword in above works similar like C++ explicit constructor??? to be confirm. c++ allow `implicit` type operator. 

#### Java
For numeric types, only implicit widening is permited, to narrow down, implicit conversion must be done via explicit expression
```java
int myInt = (int) myDouble;
byte myByte = (byte) myInt;
```

In java, there is no operator overloading. If explicit keyword apply to all C++ single parameter constructor, 

### Is there any infrastructure in C++20

`std::type_identity` in C++20 does not help, `void g(T, std::type_identity_t<T>);`

will c++20 concept `requires` can help?

## Existing solutions

### delete the template function
The macro below can prevent implicit conversion for all parameters (needs C++11)
<http://www.gockelhut.com/cpp-pirate/disable-implicit-casts.html>

```cpp
#define DISABLE_IMPLICIT_CASTS(func_name) \
    template <typename... T>              \
    void func_name(T...) = delete
```

**Limitation**
 too complicated if users want to prevent single parameter implicit conversion but not all.

### type_trait can narrow down the types to a limited set 
```cpp
struct Money {};

template <typename T>
Money & operator*=(const T & n)
{
  static_assert(std::is_integral<T>::value, "Error: can only multiply money by integral amounts!");
  // ...
}
```
**Limitation**  it is flexible, but need extra work.

### `nothing_but<T>`
The source can not download any more, but blog/documentation was retrieved, [add ref]

**Limitation**
only apply to limited numeric types which have been specialized


## Proposed solution

Reuse the keyword `explicit` to prevent implicit conversion

```c++
explicit void f(int i, double j); // prevent implicit conversion of all parameters 

void g(explicit int i, double j); // prevent implicit conversion for the first parameter
```

## implementation 

Implementation in compiler should be straight-forwards, actually, this reduce compiler's work to match overloading functions. 

<https://riptutorial.com/cplusplus/example/8117/steps-of-overload-resolution>