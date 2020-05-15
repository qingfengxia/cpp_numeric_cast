# Runtime safe getter for Array and non-hash STL sequence containers

very early draft for discussion

By Qingfeng Xia 2020

## Motivation

Safe getter `std::get<size_t_index>(ContainerType)` is available for `std::tuple` and `std::array<ElementType, fixed_size>`,  both have the known element size at the compiling time. The compiler will do bounding check at compiling time. Runtime safe indexing function `at(size_t index)` are available for `std::vector`. If index is out of bound, exception will throw. 

It may be useful to have a consistent safe getter for contiguous, random-accessible containers `std::vector`, [`std::valarray` (since c++11)](https://en.cppreference.com/w/cpp/numeric/valarray), [`std::span` (since c++20)](https://en.cppreference.com/w/cpp/container/span).  

Other containers like `std::list` which is not contiguous in memory, no safe indexing function `at()`, should be excluded.

Raw array is desired to have a `std::get()`, but may needs some special treatment, 
`array T[]` is converted to pointer when passed in as template parameter, the array size may got lost. For the moment, convert `array T[]` to `std::array<T>`. 

Note: `std::set` is a container type,  so a modifiable accessor is `T& std::get<>()` returning non-const reference.

### `std::span` has no `at()`
<https://en.cppreference.com/w/cpp/container/span>  

c++20  `std::array` from builtin array by `std::to_array()` in C++20.

### `std::begin()` and iterator 

`iterator` can be used with `std::list`, even non-contiguous memory, raw pointer can not accessor the next element by offset pointer like `pointer++`.

## Implementation 
todo


